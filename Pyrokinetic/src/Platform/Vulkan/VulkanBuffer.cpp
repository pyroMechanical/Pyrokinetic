#include "pkpch.h"
#include "VulkanBuffer.h"
#include <vulkan/vulkan.h>

namespace pk
{
#define CHECK_VULKAN(x) \
		{ \
			VkResult err = x;								\
			if (err)										\
			{												\
				PK_CORE_ERROR("Vulkan Error: {0}", err);	\
			}												\
		}													\

	//	VertexBuffer ///////////////////////////////////////////////////////////////

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
	{
		PROFILE_FUNCTION();

		//glCreateBuffers(1, &m_RendererID);
		//glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		//glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		allocator = VulkanMemory::GetInstance()->GetAllocator();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		CHECK_VULKAN(vmaCreateBuffer(*allocator, &bufferInfo, &vmaallocInfo, &m_Buffer.buffer, &m_Buffer.allocation, nullptr));
	}

	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
	{
		PROFILE_FUNCTION();

		//glCreateBuffers(1, &m_RendererID);
		//glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		//glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

		allocator = VulkanMemory::GetInstance()->GetAllocator();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		CHECK_VULKAN(vmaCreateBuffer(*allocator, &bufferInfo, &vmaallocInfo, &m_Buffer.buffer, &m_Buffer.allocation, nullptr));

		void* region;

		vmaMapMemory(*allocator, m_Buffer.allocation, &region);
		memcpy(region, vertices, size);
		vmaUnmapMemory(*allocator, m_Buffer.allocation);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		PROFILE_FUNCTION();

		vmaDestroyBuffer(*allocator, m_Buffer.buffer, m_Buffer.allocation);
	}

	void VulkanVertexBuffer::Bind() const
	{
		PROFILE_FUNCTION();

		//glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void VulkanVertexBuffer::Unbind() const
	{
		PROFILE_FUNCTION();

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
		void* region;

		vmaMapMemory(*allocator, m_Buffer.allocation, &region);
		memcpy(region, data, size);
		vmaUnmapMemory(*allocator, m_Buffer.allocation);
	}

	VulkanVertexBuffer::VertexInputDescription VulkanVertexBuffer::GetVertexInputDescription()
	{
		VertexInputDescription description;

		VkVertexInputBindingDescription mainBinding = {};
		mainBinding.binding = 0;
		mainBinding.stride = m_Layout.GetStride();
		mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		description.m_Bindings.push_back(mainBinding);
		uint32_t location = 0;
		uint32_t offset = 0;
		for(BufferElement& element : m_Layout)
		{
			VkFormat format;
			uint8_t size = 0;
			uint8_t repeats = 0;
			switch(element.Type)
			{
				case ShaderDataType::Float:  format = VK_FORMAT_R32_SFLOAT;          size = 4;                  break;
				case ShaderDataType::Float2: format = VK_FORMAT_R32G32_SFLOAT;       size = 4 * 2;              break;
				case ShaderDataType::Float3: format = VK_FORMAT_R32G32B32_SFLOAT;    size = 4 * 3;              break;
				case ShaderDataType::Float4: format = VK_FORMAT_R32G32B32A32_SFLOAT; size = 4 * 4;              break;
				case ShaderDataType::Mat3:   format = VK_FORMAT_R32G32B32_SFLOAT;    size = 4 * 3; repeats = 2; break;
				case ShaderDataType::Mat4:   format = VK_FORMAT_R32G32B32A32_SFLOAT; size = 4 * 4; repeats = 3; break;
				case ShaderDataType::Int:    format = VK_FORMAT_R32_SINT;            size = 4;                  break;
				case ShaderDataType::Int2:   format = VK_FORMAT_R32G32_SINT;         size = 4 * 2;              break;
				case ShaderDataType::Int3:   format = VK_FORMAT_R32G32B32_SINT;      size = 4 * 3;              break;
				case ShaderDataType::Int4:   format = VK_FORMAT_R32G32B32A32_SINT;   size = 4 * 4;              break;
				case ShaderDataType::Bool:   format = VK_FORMAT_R8_UINT;			 size = 1;                  break;
			}
			do
			{
				VkVertexInputAttributeDescription attribute = {};
				attribute.binding = 0;
				attribute.location = location;
				attribute.format = format;
				attribute.offset = size;
				description.m_Attributes.push_back(attribute);
				++location;
				--repeats;
			} while (repeats > 0);
		}

		return description;
	}


	//	IndexBuffer ///////////////////////////////////////////////////////////////


	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		PROFILE_FUNCTION();

		//glCreateBuffers(1, &m_RendererID);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

		allocator = VulkanMemory::GetInstance()->GetAllocator();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = count * sizeof(uint32_t);
		bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		CHECK_VULKAN(vmaCreateBuffer(*allocator, &bufferInfo, &vmaallocInfo, &m_Buffer.buffer, &m_Buffer.allocation, nullptr));

	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		PROFILE_FUNCTION();

		vmaDestroyBuffer(*allocator, m_Buffer.buffer, m_Buffer.allocation);
	}

	void VulkanIndexBuffer::Bind() const
	{
		PROFILE_FUNCTION();

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void VulkanIndexBuffer::Unbind() const
	{
		PROFILE_FUNCTION();

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
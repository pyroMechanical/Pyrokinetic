#include "pkpch.h"
#include "VulkanBuffer.h"
#include "Pyrokinetic/Rendering/Renderer.h"
#include "VulkanContext.h"
#include <vulkan/vulkan.h>

namespace pk
{

	//	VertexBuffer ///////////////////////////////////////////////////////////////

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
	{
		PROFILE_FUNCTION();

		m_LocalBuffer = new char[size];

		VulkanContext* context = dynamic_cast<VulkanContext*>(Renderer::GetContext());

		allocator = context->GetAllocator();

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
		
		VulkanContext* context = dynamic_cast<VulkanContext*>(Renderer::GetContext());

		allocator = context->GetAllocator();

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

		delete[] m_LocalBuffer;
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

	void VulkanVertexBuffer::Unmap(uint32_t size)
	{
		void* region;

		vmaMapMemory(*allocator, m_Buffer.allocation, &region);
		memcpy(region, m_LocalBuffer, size);
		vmaUnmapMemory(*allocator, m_Buffer.allocation);
	}


	//	IndexBuffer ///////////////////////////////////////////////////////////////


	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		PROFILE_FUNCTION();

		//glCreateBuffers(1, &m_RendererID);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

		VulkanContext* context = dynamic_cast<VulkanContext*>(Renderer::GetContext());

		allocator = context->GetAllocator();

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = count * sizeof(uint32_t);
		bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		CHECK_VULKAN(vmaCreateBuffer(*allocator, &bufferInfo, &vmaallocInfo, &m_Buffer.buffer, &m_Buffer.allocation, nullptr));

		void* region;

		vmaMapMemory(*allocator, m_Buffer.allocation, &region);
		memcpy(region, indices, count*sizeof(uint32_t));
		vmaUnmapMemory(*allocator, m_Buffer.allocation);

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
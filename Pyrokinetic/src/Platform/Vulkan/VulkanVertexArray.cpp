#include "pkpch.h"
#include "VulkanVertexArray.h"

#include <vulkan/vulkan.h>

namespace pk
{
	static VkFormat ShaderDataTypeToVulkanBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:  return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::Mat3:   return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::Mat4:   return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::Int:    return VK_FORMAT_R32_SINT;
			case ShaderDataType::Int2:   return VK_FORMAT_R32G32_SINT;
			case ShaderDataType::Int3:   return VK_FORMAT_R32G32B32_SINT;
			case ShaderDataType::Int4:   return VK_FORMAT_R32G32B32A32_SINT;
			case ShaderDataType::Bool:   return VK_FORMAT_R8_UINT;
		}

		PK_CORE_ASSERT(false, "Unknown ShaderDataType!")
			return VK_FORMAT_UNDEFINED;
	}

	VulkanVertexArray::VulkanVertexArray()
	{
		PROFILE_FUNCTION();

		//glCreateVertexArrays(1, &m_RendererID);
	}
	VulkanVertexArray::~VulkanVertexArray() 
	{
		PROFILE_FUNCTION();

		//glDeleteVertexArrays(1, &m_RendererID);
	}

	void VulkanVertexArray::Bind() const
	{
		PROFILE_FUNCTION();

		//glBindVertexArray(m_RendererID);
	}

	void VulkanVertexArray::Unbind() const
	{
		PROFILE_FUNCTION();

		//glBindVertexArray(0);
	}

	void VulkanVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		PROFILE_FUNCTION();
		/*
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		PK_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, element.GetComponentCount(),
				ShaderDataTypeToVulkanBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(), (const void*) element.Offset);
			index++;
		}
		*/
		m_VertexBuffers.push_back(vertexBuffer);
		
	}

	void VulkanVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		PROFILE_FUNCTION();

		//glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;

	}
}
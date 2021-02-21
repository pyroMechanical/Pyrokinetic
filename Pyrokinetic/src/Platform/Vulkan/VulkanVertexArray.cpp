#include "pkpch.h"
#include "VulkanVertexArray.h"

#include <vulkan/vulkan.h>

namespace Pyrokinetic
{
	static GLenum ShaderDataTypeToVulkanBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case Pyrokinetic::ShaderDataType::Float:   return GL_FLOAT;
			case Pyrokinetic::ShaderDataType::Float2:  return GL_FLOAT;
			case Pyrokinetic::ShaderDataType::Float3:  return GL_FLOAT;
			case Pyrokinetic::ShaderDataType::Float4:  return GL_FLOAT;
			case Pyrokinetic::ShaderDataType::Mat3:    return GL_FLOAT;
			case Pyrokinetic::ShaderDataType::Mat4:    return GL_FLOAT;
			case Pyrokinetic::ShaderDataType::Int:     return GL_INT;
			case Pyrokinetic::ShaderDataType::Int2:    return GL_INT;
			case Pyrokinetic::ShaderDataType::Int3:    return GL_INT;
			case Pyrokinetic::ShaderDataType::Int4:    return GL_INT;
			case Pyrokinetic::ShaderDataType::Bool:    return GL_BOOL;
		}

		PK_CORE_ASSERT(false, "Unknown ShaderDataType!")
			return 0;
	}

	VulkanVertexArray::VulkanVertexArray()
	{
		PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}
	VulkanVertexArray::~VulkanVertexArray() 
	{
		PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void VulkanVertexArray::Bind() const
	{
		PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void VulkanVertexArray::Unbind() const
	{
		PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void VulkanVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		PROFILE_FUNCTION();

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

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VulkanVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;

	}
}
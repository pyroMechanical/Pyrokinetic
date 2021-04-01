#pragma once

#include "Pyrokinetic/Rendering/Buffer.h"

#include <glad/glad.h>

namespace pk
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(void* data, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void* Map() override { return m_LocalBuffer; }
		virtual void Unmap(uint32_t size = 0) override;

		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }

		uint32_t GetRendererID() const { return m_RendererID; }
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
		uint32_t m_Size;
		char* m_LocalBuffer = nullptr;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const { return m_Count; }

		uint32_t GetRendererID() const { return m_RendererID; }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

	inline GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case pk::ShaderDataType::Float:   return GL_FLOAT;
		case pk::ShaderDataType::Float2:  return GL_FLOAT;
		case pk::ShaderDataType::Float3:  return GL_FLOAT;
		case pk::ShaderDataType::Float4:  return GL_FLOAT;
		case pk::ShaderDataType::Mat3:    return GL_FLOAT;
		case pk::ShaderDataType::Mat4:    return GL_FLOAT;
		case pk::ShaderDataType::Int:     return GL_INT;
		case pk::ShaderDataType::Int2:    return GL_INT;
		case pk::ShaderDataType::Int3:    return GL_INT;
		case pk::ShaderDataType::Int4:    return GL_INT;
		case pk::ShaderDataType::Bool:    return GL_BOOL;
		}

		PK_CORE_ASSERT(false, "Unknown ShaderDataType!")
			return 0;
	}
}
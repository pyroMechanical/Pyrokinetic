#include "pkpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/Vulkan/VulkanBuffer.h"

namespace pk
{

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case API::OpenGL:     return std::make_shared<OpenGLVertexBuffer>(size);
		case API::Vulkan:     return std::make_shared<VulkanVertexBuffer>(size);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case API::OpenGL:     return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		case API::Vulkan:     return std::make_shared<VulkanVertexBuffer>(vertices, size);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case API::OpenGL:    return std::make_shared<OpenGLIndexBuffer>(indices, count);
		case API::Vulkan:     return std::make_shared<VulkanIndexBuffer>(indices, count);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
#include "pkpch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace pk
{
	std::shared_ptr<VertexArray> VertexArray::Create()
	{

		switch (Renderer::GetAPI())
		{
		case API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case API::OpenGL:     return std::make_shared<OpenGLVertexArray>();
		case API::Vulkan:    return std::make_shared<VulkanVertexArray>();
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
		
	}
}
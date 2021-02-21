#include "pkpch.h"
#include "GraphicsContext.h"

#include "Pyrokinetic/Rendering/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace Pyrokinetic
{
	std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case API::OpenGL:     return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(window));
		case API::Vulkan:     return std::make_unique<VulkanContext>(static_cast<GLFWwindow*>(window));
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
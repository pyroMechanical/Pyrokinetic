#include "pkpch.h"
#include "Framebuffer.h"

#include "Pyrokinetic/Rendering/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"

namespace pk
{
	
	std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: PK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPI::API::OpenGL: return std::make_unique<OpenGLFramebuffer>(spec);
		case RendererAPI::API::Vulkan: return std::make_unique<VulkanFramebuffer>(spec);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
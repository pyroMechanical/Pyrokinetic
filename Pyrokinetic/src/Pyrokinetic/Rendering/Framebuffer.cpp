#include "pkpch.h"
#include "Framebuffer.h"

#include "Pyrokinetic/Rendering/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace pk
{
	
	std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case API::None: PK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case API::OpenGL: return std::make_unique<OpenGLFramebuffer>(spec);
		//case API::Vulkan: return std::make_unique<VulkanFramebuffer>(spec);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
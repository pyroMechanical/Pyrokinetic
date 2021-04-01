#include "pkpch.h"
#include "RenderPass.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRenderPass.h"
#include "Platform/Vulkan/VulkanRenderPass.h"

namespace pk
{
	std::shared_ptr<RenderPass> RenderPass::Create(RenderPassSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: PK_CORE_ASSERT(false, "None not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLRenderPass>(spec);
		case RendererAPI::API::Vulkan: return std::make_shared<VulkanRenderPass>(spec);
		}

		PK_CORE_ASSERT(false, "RenderAPI not supported!");
		return nullptr;
	}
}
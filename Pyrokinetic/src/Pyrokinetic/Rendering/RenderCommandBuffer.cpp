#include "pkpch.h"
#include "RenderCommandBuffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLRenderCommandBuffer.h"
#include "Platform/Vulkan/VulkanRenderCommandBuffer.h"


namespace pk
{
	std::shared_ptr<RenderCommandBuffer> RenderCommandBuffer::Create()
	{
		switch(RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: PK_CORE_ASSERT(false, "None not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLRenderCommandBuffer>();
		case RendererAPI::API::Vulkan: return std::make_shared<VulkanRenderCommandBuffer>();
		}

		PK_CORE_ASSERT(false, "RenderAPI not supported!");
		return nullptr;
	}
}
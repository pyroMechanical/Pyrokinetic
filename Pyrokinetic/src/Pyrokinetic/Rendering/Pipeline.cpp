#include "pkpch.h"
#include "Pipeline.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLPipeline.h"
#include "Platform/Vulkan/VulkanPipeline.h"

namespace pk
{
	std::shared_ptr<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None: PK_CORE_ASSERT(false, "None not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLPipeline>(spec);
		case RendererAPI::API::Vulkan: return std::make_shared<VulkanPipeline>(spec);
		}

		PK_CORE_ASSERT(false, "RenderAPI not supported!");
		return nullptr;
	}
}
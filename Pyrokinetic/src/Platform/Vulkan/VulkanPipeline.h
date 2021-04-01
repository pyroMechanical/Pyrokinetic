#pragma once

#include "Pyrokinetic/Rendering/Pipeline.h"

#include <vulkan/vulkan.h>

namespace pk
{
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification& spec);
		virtual ~VulkanPipeline() override {};

		virtual const PipelineSpecification& GetSpecification() override { return m_Spec; }

		VkPipeline GetVulkanPipeline() { return m_Pipeline; }

	private:
		PipelineSpecification m_Spec;
		VkPipeline m_Pipeline;
	};
}
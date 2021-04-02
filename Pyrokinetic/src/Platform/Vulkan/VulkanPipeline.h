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

		virtual void Rebuild() override;

		VkPipeline GetVulkanPipeline() { return m_Pipeline; }

	private:
		PipelineSpecification m_Spec;
		VkPipelineLayout m_Layout;
		VkPipeline m_Pipeline;
	};
}
#pragma once

#include "Pyrokinetic/Rendering/Pipeline.h"
#include "VulkanShader.h"
#include <vulkan/vulkan.h>

namespace pk
{
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification& spec);
		virtual ~VulkanPipeline() override {};

		virtual const PipelineSpecification& GetSpecification() override { return m_Spec; }

		const VkPipelineLayout& GetPipelineLayout() const { return m_Layout; }

		virtual void Rebuild() override;

		std::vector<VkDescriptorSet> GetPipelineDescriptorSets() { return m_DescriptorSet.DescriptorSets; }

		void WriteImageSamplers();

		VkPipeline GetVulkanPipeline() { return m_Pipeline; }

	private:
		PipelineSpecification m_Spec;
		VkPipelineLayout m_Layout;
		VkPipeline m_Pipeline;

		VulkanShader::ShaderMaterialDescriptorSet m_DescriptorSet;
	};
}
#pragma once
#include <vulkan/vulkan.h>
#include <vector>
namespace pk
{
	class VulkanPipelineBuilder
	{
	public:
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
		VkPipelineVertexInputStateCreateInfo m_VertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo m_InputAssembly;
		VkViewport m_Viewport;
		VkRect2D m_Scissor;
		VkPipelineRasterizationStateCreateInfo m_Rasterizer;
		VkPipelineColorBlendAttachmentState m_ColorBlendAttachment;
		VkPipelineMultisampleStateCreateInfo m_Multisampling;
		VkPipelineLayout m_PipelineLayout;

		VkPipeline BuildPipeline(VkDevice device, VkRenderPass pass);
	};
}
#include "pkpch.h"
#include "VulkanPipeline.h"
#include "VulkanContext.h"
#include "VulkanInitializer.h"

namespace pk
{
	VulkanPipeline::VulkanPipeline(const PipelineSpecification& spec)
		: m_Spec(spec)
	{
		Rebuild();
	}

	void VulkanPipeline::Rebuild()
	{
		VulkanContext* context = VulkanContext::Get();

		auto VkShader = std::dynamic_pointer_cast<VulkanShader>(m_Spec.Shader);
		auto VkRenderPass = std::dynamic_pointer_cast<VulkanRenderPass>(m_Spec.RenderPass);

		VkExtent2D viewportExtent = context->GetSwapchain().GetExtent();

		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = viewportExtent.width;
		viewport.height = viewportExtent.height;
		viewport.minDepth = 1.0f;
		viewport.maxDepth = 0.0f;
		
		VkRect2D scissor = {};
		scissor.offset = { 0,0 };
		scissor.extent = viewportExtent;


		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;

		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.pNext = nullptr;

		auto attachmentState = vkinit::color_blend_attachment_state();

		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &attachmentState;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		auto shaderModules = VkShader->GetShaderModules();

		for(auto it = shaderModules.begin(); it != shaderModules.end(); ++it)
		{
			VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
			shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCreateInfo.pNext = nullptr;
			shaderStageCreateInfo.stage = it->first;
			shaderStageCreateInfo.module = it->second;
			shaderStageCreateInfo.flags = 0;
			shaderStageCreateInfo.pName = "main";
			shaderStageCreateInfo.pSpecializationInfo = nullptr;

			shaderStages.push_back(shaderStageCreateInfo);
		}
		
		
		auto inputAssemblyState = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		auto rasterizationState = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);
		auto multisamplingState = vkinit::multisampling_state_create_info();

		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;

		VkVertexInputBindingDescription binding = {};
		binding.binding = 0;
		binding.stride = m_Spec.vertexBufferLayout.GetStride();
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		bindings.push_back(binding);

		auto bufferElements = m_Spec.vertexBufferLayout.GetElements();
		size_t i = 0;
		for (auto element : bufferElements)
		{

			VkVertexInputAttributeDescription attribute = {};
			attribute.binding = 0;
			attribute.location = i;
			VkFormat format = VK_FORMAT_UNDEFINED;
			switch (element.Type)
			{
			case ShaderDataType::Float:  format = VK_FORMAT_R32_SFLOAT; break;
			case ShaderDataType::Float2: format = VK_FORMAT_R32G32_SFLOAT; break;
			case ShaderDataType::Float3: format = VK_FORMAT_R32G32B32_SFLOAT; break;
			case ShaderDataType::Float4: format = VK_FORMAT_R32G32B32A32_SFLOAT; break;
			case ShaderDataType::Int:    format = VK_FORMAT_R32_SINT; break;
			case ShaderDataType::Int2:   format = VK_FORMAT_R32G32_SINT; break;
			case ShaderDataType::Int3:   format = VK_FORMAT_R32G32B32_SINT; break;
			case ShaderDataType::Int4:   format = VK_FORMAT_R32G32B32A32_SINT; break;
			case ShaderDataType::Bool:   format = VK_FORMAT_R8_UINT; break;
			}
			attribute.format = format;
			attribute.offset = element.Offset;

			attributes.push_back(attribute);
			format = VK_FORMAT_UNDEFINED;
			++i;
		}

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = nullptr;

		vertexInputStateCreateInfo.vertexBindingDescriptionCount = bindings.size();
		vertexInputStateCreateInfo.pVertexBindingDescriptions = bindings.data();
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributes.size();
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributes.data();

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = nullptr;
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputStateCreateInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizationState;
		pipelineInfo.pMultisampleState = &multisamplingState;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = VkShader->GetPipelineLayout();
		pipelineInfo.renderPass = VkRenderPass->GetVulkanRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		CHECK_VULKAN(vkCreateGraphicsPipelines(context->GetDevice()->GetVulkanDevice(), nullptr, 1, &pipelineInfo, nullptr, &m_Pipeline)); //current is 0xFFFFFFFFFFFFFFFE?
	}
}
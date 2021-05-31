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
		PROFILE_FUNCTION();

		VulkanContext* context = VulkanContext::Get();

		VkDevice device = context->GetDevice()->GetVulkanDevice();

		auto VkShader = std::dynamic_pointer_cast<VulkanShader>(m_Spec.Shader);
		auto VkRenderPass = std::dynamic_pointer_cast<VulkanRenderPass>(m_Spec.RenderPass);

		auto descriptorSetLayouts = VkShader->GetAllDescriptorSetLayouts();

		const auto& pushConstantRanges = VkShader->GetPushConstantRanges();

		std::vector<VkPushConstantRange> vkPushConstantRanges(pushConstantRanges.size());
		for (uint32_t i = 0; i < pushConstantRanges.size(); i++)
		{
			const auto& pushConstantRange = pushConstantRanges[i];
			auto& vkPushConstantRange = vkPushConstantRanges[i];

			vkPushConstantRange.stageFlags = pushConstantRange.ShaderStage;
			vkPushConstantRange.offset = pushConstantRange.Offset;
			vkPushConstantRange.size = pushConstantRange.Size;
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayouts.size();
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = vkPushConstantRanges.size();
		pipelineLayoutCreateInfo.pPushConstantRanges = vkPushConstantRanges.data();

		CHECK_VULKAN(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &m_Layout));

		VkExtent2D viewportExtent = context->GetSwapchain().GetExtent();

		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = viewportExtent.width;
		viewport.height = viewportExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		
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
		
		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = VkShader->GetPipelineShaderStageCreateInfos();
		
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

		VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
		std::vector<VkDynamicState> states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.pNext = nullptr;
		dynamicStateInfo.dynamicStateCount = 2;
		dynamicStateInfo.pDynamicStates = states.data();
		
		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = nullptr;
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputStateCreateInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizationState;
		pipelineInfo.pDynamicState = &dynamicStateInfo;
		pipelineInfo.pMultisampleState = &multisamplingState;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.layout = m_Layout;
		pipelineInfo.renderPass = VkRenderPass->GetVulkanRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		CHECK_VULKAN(vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineInfo, nullptr, &m_Pipeline));

		m_DescriptorSet = VkShader->CreateDescriptorSets();
		std::vector<VkWriteDescriptorSet> writeDescriptors;

		const auto& shaderDescriptorSets = VkShader->GetShaderDescriptorSets();
		for (auto&& [set, shaderDescriptorSet] : shaderDescriptorSets)
		{
			for (auto&& [binding, uniformBuffer] : shaderDescriptorSet.UniformBuffers)
			{
				VkWriteDescriptorSet writeDescriptorSet = {};
				writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSet.descriptorCount = 1;
				writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				writeDescriptorSet.pBufferInfo = &uniformBuffer->Descriptor;
				writeDescriptorSet.dstBinding = binding;
				writeDescriptorSet.dstSet = m_DescriptorSet.DescriptorSets[0];
				writeDescriptors.push_back(writeDescriptorSet);
			}
		}

		//PK_CORE_WARN("Vulkan Pipeline - Updating {0} descriptor sets", writeDescriptors.size());
		vkUpdateDescriptorSets(device, writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
	}

	void VulkanPipeline::WriteImageSamplers()
	{
		PROFILE_FUNCTION();
		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

		auto VkShader = std::dynamic_pointer_cast<VulkanShader>(m_Spec.Shader);

		auto imageInfos = VkShader->GetImageInfos();
		const auto& shaderDescriptorSets = VkShader->GetShaderDescriptorSets();
		std::vector<VkWriteDescriptorSet> writeDescriptors;

		for (auto&& [set, shaderDescriptorSet] : shaderDescriptorSets)
		{
			for (auto&& [binding, imageSampler] : shaderDescriptorSet.ImageSamplers)
			{
				VkWriteDescriptorSet writeDescriptorSet = {};
				writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSet.pNext = nullptr;
				writeDescriptorSet.dstBinding = imageSampler.Binding;
				writeDescriptorSet.dstSet = m_DescriptorSet.DescriptorSets[imageSampler.DescriptorSet];
				writeDescriptorSet.descriptorCount = imageInfos.size() <= imageSampler.Count ? imageInfos.size() : imageSampler.Count;
				writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				writeDescriptorSet.pImageInfo = imageInfos.data();
				writeDescriptors.push_back(writeDescriptorSet);
			}
		}

		PK_CORE_WARN("Vulkan Pipeline - Updating {0} descriptor sets", writeDescriptors.size());
		vkUpdateDescriptorSets(device, writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
	}
}
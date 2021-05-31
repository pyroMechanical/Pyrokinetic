#include "pkpch.h"
#include "VulkanRenderCommandBuffer.h"
#include "VulkanContext.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"

#include <vulkan/vulkan.h>
#include "Pyrokinetic/Rendering/Renderer.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"

namespace pk
{
	VulkanRenderCommandBuffer::VulkanRenderCommandBuffer()
	{
		
	}

	VulkanRenderCommandBuffer::~VulkanRenderCommandBuffer()
	{
		
	}

	void VulkanRenderCommandBuffer::BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass)
	{
		PROFILE_FUNCTION();
		VulkanRenderPass* vkRenderPass = static_cast<VulkanRenderPass*>(renderPass.get());

		m_RenderPasses.push_back(vkRenderPass);

		m_Queue.push_back([=](const VkCommandBuffer& drawCommandBuffer, const VulkanFramebuffer& framebuffer)
			{
				PROFILE_FUNCTION();
				VulkanContext* context = dynamic_cast<VulkanContext*>(Renderer::GetContext());

				auto& clearColor = framebuffer.GetSpecification().renderPass->GetSpecification().clearColor;

				VkClearValue clearValues[2];
				clearValues[0].color = { {clearColor.r, clearColor.g, clearColor.b, clearColor.a} };
				clearValues[1].depthStencil = { 1.0f, 0 };

				VkRenderPassBeginInfo renderPassBeginInfo = {};
				renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassBeginInfo.pNext = nullptr;
				renderPassBeginInfo.renderPass = vkRenderPass->GetVulkanRenderPass();
				renderPassBeginInfo.renderArea.offset.x = 0;
				renderPassBeginInfo.renderArea.offset.y = 0;
				renderPassBeginInfo.renderArea.extent.width = framebuffer.GetSpecification().width;
				renderPassBeginInfo.renderArea.extent.height = framebuffer.GetSpecification().height;
				renderPassBeginInfo.clearValueCount = 2;
				renderPassBeginInfo.pClearValues = clearValues;
				renderPassBeginInfo.framebuffer = framebuffer.GetVulkanFramebuffer();

				vkCmdBeginRenderPass(drawCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			});
	}

	void VulkanRenderCommandBuffer::EndRenderPass()
	{
		PROFILE_FUNCTION();
		m_Queue.push_back([=](const VkCommandBuffer& drawCommandBuffer, const VulkanFramebuffer& framebuffer)
			{
				PROFILE_FUNCTION();
				vkCmdEndRenderPass(drawCommandBuffer);
			});
	}

	void VulkanRenderCommandBuffer::Submit(const std::shared_ptr<Pipeline>& pipeline,
		const std::shared_ptr<VertexBuffer>& vertexBuffer,
		const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t indexCount)
	{
		PROFILE_FUNCTION();

		VulkanContext* context = VulkanContext::Get();

		m_Queue.push_back([=](const VkCommandBuffer& drawCommandBuffer, const VulkanFramebuffer& framebuffer)
			{
				PROFILE_FUNCTION();

				VulkanPipeline* vkPipeline = static_cast<VulkanPipeline*>(pipeline.get());
				vkPipeline->WriteImageSamplers();
				auto& descriptorSet = vkPipeline->GetPipelineDescriptorSets();
				vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->GetPipelineLayout(), 0, descriptorSet.size(), descriptorSet.data(), 0, nullptr);

				vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->GetVulkanPipeline());

				PK_CORE_INFO("Viewport Size: {0}, {1}", framebuffer.GetSpecification().width, framebuffer.GetSpecification().height);
				VkViewport viewport = {};
				viewport.x = 0;
				viewport.y = 0;
				viewport.width = (float)framebuffer.GetSpecification().width;
				viewport.height = (float)framebuffer.GetSpecification().height;
				viewport.minDepth = (float)0.0f;
				viewport.maxDepth = (float)1.0f;
				vkCmdSetViewport(drawCommandBuffer, 0, 1, &viewport);

				VkRect2D scissor = {};
				scissor.extent.width = framebuffer.GetSpecification().width;
				scissor.extent.height = framebuffer.GetSpecification().height;
				scissor.offset.x = 0;
				scissor.offset.y = 0;
				vkCmdSetScissor(drawCommandBuffer, 0, 1, &scissor);

				VkDeviceSize offsets[1] = { 0 };
				VulkanVertexBuffer* vkVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer.get());
				vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &vkVertexBuffer->GetDeviceBuffer(), offsets);

				VulkanIndexBuffer* vkIndexBuffer = static_cast<VulkanIndexBuffer*>(indexBuffer.get());
				vkCmdBindIndexBuffer(drawCommandBuffer, vkIndexBuffer->GetDeviceBuffer(), 0, VK_INDEX_TYPE_UINT32);

				vkCmdDrawIndexed(drawCommandBuffer, indexCount, 1, 0, 0, 1);
			});
	}

	void VulkanRenderCommandBuffer::Flush()
	{
		PROFILE_FUNCTION();

		VulkanContext* context = VulkanContext::Get();
		auto& device = context->GetDevice();

		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.pNext = nullptr;

		for (auto renderPass : m_RenderPasses)
		{
			auto& framebuffers = renderPass->GetFramebuffers();
			for (auto framebuffer : framebuffers)
			{
				auto cmd = device->GetCommandBuffer(true, false);

				for (auto& func : m_Queue)
					func(cmd.buffer, *framebuffer);

				device->EndCommandBuffer(cmd, true);
			}
		}

 		m_Queue.clear();
		m_RenderPasses.clear();
	}
}

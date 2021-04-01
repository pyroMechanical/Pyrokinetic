#include "pkpch.h"
#include "VulkanRenderCommandBuffer.h"
#include "VulkanContext.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanRenderPass.h"

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
		VulkanRenderPass* vkRenderPass = static_cast<VulkanRenderPass*>(renderPass.get());

		m_Queue.push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
			{
				VulkanContext* context = dynamic_cast<VulkanContext*>(Renderer::GetContext());

				auto& clearColor = vkRenderPass->GetSpecification().TargetFramebuffer->GetSpecification().clearColor;

				VkClearValue clearValues[2];
				clearValues[0].color = { {clearColor.r, clearColor.g, clearColor.b, clearColor.a} };
				clearValues[1].depthStencil = { 1.0f, 0 };

				VkRenderPassBeginInfo renderPassBeginInfo = {};
				renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassBeginInfo.pNext = nullptr;
				renderPassBeginInfo.renderPass = context->GetRenderPass();
				renderPassBeginInfo.renderArea.offset.x = 0;
				renderPassBeginInfo.renderArea.offset.y = 0;
				renderPassBeginInfo.renderArea.extent.width = context->GetWidth();
				renderPassBeginInfo.renderArea.extent.height = context->GetHeight();
				renderPassBeginInfo.clearValueCount = 2;
				renderPassBeginInfo.pClearValues = clearValues;
				renderPassBeginInfo.framebuffer = framebuffer;

				vkCmdBeginRenderPass(drawCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			});
	}

	void VulkanRenderCommandBuffer::EndRenderPass()
	{
		m_Queue.push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
			{
				//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), drawCommandBuffer);

				vkCmdEndRenderPass(drawCommandBuffer);
			});
	}

	void VulkanRenderCommandBuffer::Submit(const std::shared_ptr<Pipeline>& pipeline,
		const std::shared_ptr<VertexBuffer>& vertexBuffer,
		const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t indexCount)
	{
		VulkanContext* context = dynamic_cast<VulkanContext*>(Renderer::GetContext());

		m_Queue.push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
			{
				VkViewport viewport = {};
				viewport.x = 0;
				viewport.y = (float)context->GetHeight();
				viewport.width = (float)context->GetWidth();
				viewport.height = -(float)context->GetHeight();
				viewport.minDepth = (float)0.0f;
				viewport.maxDepth = (float)1.0f;
				vkCmdSetViewport(drawCommandBuffer, 0, 1, &viewport);

				VkRect2D scissor = {};
				scissor.extent.width = context->GetWidth();
				scissor.extent.height = context->GetHeight();
				scissor.offset.x = 0;
				scissor.offset.y = 0;
				vkCmdSetScissor(drawCommandBuffer, 0, 1, &scissor);

				VulkanPipeline* vkPipeline = static_cast<VulkanPipeline*>(pipeline.get());
				VulkanShader* shader = dynamic_cast<VulkanShader*>(vkPipeline->GetSpecification().Shader.get());
				//vkCmdBindDescriptorSets(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipelineLayout(), 0, 1, &shader->GetDescriptorSet(), 0, nullptr);

				//vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->GetVulkanPipeline());
				vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context->GetPipeline());

				VkDeviceSize offsets[1] = { 0 };
				VulkanVertexBuffer* vkVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer.get());
				vkCmdBindVertexBuffers(drawCommandBuffer, 0, 1, &vkVertexBuffer->GetDeviceBuffer(), offsets);

				VulkanIndexBuffer* vkIndexBuffer = static_cast<VulkanIndexBuffer*>(indexBuffer.get());
				vkCmdBindIndexBuffer(drawCommandBuffer, vkIndexBuffer->GetDeviceBuffer(), 0, VK_INDEX_TYPE_UINT32);

				vkCmdDrawIndexed(drawCommandBuffer, indexCount == 0 ? indexBuffer->GetCount() : indexCount, 1, 0, 0, 1);
			});
	}

	void VulkanRenderCommandBuffer::Flush()
	{
		VulkanContext* context = dynamic_cast<VulkanContext*>(Renderer::GetContext());
		auto& device = context->GetDevice();

		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.pNext = nullptr;

		auto& drawCmdBuffers = context->GetCommandBuffers();
		auto& framebuffers = context->GetFramebuffers();

		for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
		{
			CHECK_VULKAN(vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBeginInfo));

			/*for (auto& func : m_Queue)
				func(drawCmdBuffers[i], framebuffers[i]);*/

			CHECK_VULKAN(vkEndCommandBuffer(drawCmdBuffers[i]));
		}

		m_Queue.clear();
	}
}

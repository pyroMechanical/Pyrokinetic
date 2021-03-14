#include "pkpch.h"
#include "VulkanRendererAPI.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace pk
{


	void VulkanRendererAPI::Init()
	{
		PROFILE_FUNCTION();
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		PROFILE_FUNCTION();

		m_Context->SetViewport({ width - x, height - y });
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
		PROFILE_FUNCTION();

		VkClearValue clearValue;
		clearValue.color = { color.r, color.g, color.b, color.a };
		m_Context->clearValue = clearValue;
	}

	void VulkanRendererAPI::Clear()
	{
		PROFILE_FUNCTION();

		FrameData currentFrame = m_Context->GetCurrentFrame(m_Context->frameNumber);

		VkRenderPassBeginInfo rpInfo = {};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpInfo.pNext = nullptr;
		rpInfo.renderPass = m_Context->renderPass;
		rpInfo.renderArea.offset.x = 0;
		rpInfo.renderArea.offset.y = 0;
		rpInfo.renderArea.extent = m_Context->windowExtent;
		rpInfo.framebuffer = m_Context->framebuffers[m_Context->frameNumber % FRAME_OVERLAP];
		rpInfo.clearValueCount = 1;
		rpInfo.pClearValues = &m_Context->clearValue;

		vkCmdBeginRenderPass(currentFrame.mainCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdEndRenderPass(currentFrame.mainCommandBuffer);
	}

	void VulkanRendererAPI::Begin()
	{
		FrameData currentFrame = m_Context->GetCurrentFrame(m_Context->frameNumber);

		CHECK_VULKAN(vkResetCommandBuffer(currentFrame.mainCommandBuffer, 0));
		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.pNext = nullptr;
		cmdBeginInfo.pInheritanceInfo = nullptr;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CHECK_VULKAN(vkBeginCommandBuffer(currentFrame.mainCommandBuffer, &cmdBeginInfo));
	}

	void VulkanRendererAPI::End()
	{
		FrameData currentFrame = m_Context->GetCurrentFrame(m_Context->frameNumber);

		CHECK_VULKAN(vkEndCommandBuffer(currentFrame.mainCommandBuffer));
	}

	void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
	{
		PROFILE_FUNCTION();

		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		//glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}
}
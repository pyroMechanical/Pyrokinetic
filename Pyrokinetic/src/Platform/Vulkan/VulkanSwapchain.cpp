#include "pkpch.h"
#include "VulkanSwapchain.h"
#include "VulkanContext.h"

namespace pk
{
	void VulkanSwapchain::CreateSurface()
	{
		PROFILE_FUNCTION();

		glfwCreateWindowSurface(VulkanContext::GetVulkanInstance(), m_Window, nullptr, &m_Surface);
	}

	void VulkanSwapchain::CreateSwapchain(const std::shared_ptr<VulkanDevice>& device)
	{
		PROFILE_FUNCTION();

		VkDevice _device = device->GetVulkanDevice();
		VkPhysicalDevice _physicalDevice = device->GetPhysicalDevice()->GetVulkanPhysicalDevice();
		vkb::SwapchainBuilder builder{ _physicalDevice, _device, m_Surface };

		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		m_Extent = { (uint32_t)width, (uint32_t)height };

		vkb::Swapchain vkb_swapchain = builder.set_desired_format({VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
			.set_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) //change this in future to change with VSync!
			.set_desired_extent(m_Extent.width, m_Extent.height)
			.build()
			.value();

		m_Swapchain = vkb_swapchain.swapchain;
		m_SwapchainImages = vkb_swapchain.get_images().value();
		m_SwapchainImageViews = vkb_swapchain.get_image_views().value();
		m_SwapchainImageFormat = vkb_swapchain.image_format;
	}

	void VulkanSwapchain::CreateFramebuffers(const FramebufferSpecification& spec)
	{
		PROFILE_FUNCTION();

		std::shared_ptr<VulkanDevice> device = VulkanContext::Get()->GetDevice();

		for (size_t i = 0; i < m_SwapchainImageViews.size(); i++)
		{
			m_Framebuffers.push_back(std::make_shared<VulkanFramebuffer>(spec, m_SwapchainImages[i], m_SwapchainImageViews[i]));
			m_FrameCommandBuffers.push_back(device->GetCommandBuffer(true, false));
			device->EndCommandBuffer(m_FrameCommandBuffers[i], false);
		}

		m_RenderPass = std::dynamic_pointer_cast<VulkanRenderPass>(spec.renderPass);
	}

	void VulkanSwapchain::DestroyFramebuffers()
	{
		PROFILE_FUNCTION();

		std::shared_ptr<VulkanDevice> device = VulkanContext::Get()->GetDevice();

		for (auto& cmd : m_FrameCommandBuffers)
		{
			device->FlushCommandBuffer(cmd);
		}
		m_FrameCommandBuffers.clear();
		m_Framebuffers.clear();
	}

	void VulkanSwapchain::SwapBuffers()
	{
		PROFILE_FUNCTION();

		std::shared_ptr<VulkanDevice> device = VulkanContext::Get()->GetDevice();

		auto& currentFramebuffer = m_Framebuffers[m_CurrentFramebuffer];

		uint32_t swapchainImageIndex = m_CurrentFramebuffer;
		//wait for rendering to complete
		CHECK_VULKAN(vkWaitForFences(device->GetVulkanDevice(), 1, &currentFramebuffer->m_RenderFence, VK_TRUE, 1000000000));
		vkResetFences(device->GetVulkanDevice(), 1, &currentFramebuffer->m_RenderFence);

		CHECK_VULKAN(vkAcquireNextImageKHR(device->GetVulkanDevice(), m_Swapchain, 1000000000, currentFramebuffer->m_PresentSemaphore, nullptr, &swapchainImageIndex));

		//submit current frame's command buffer to the graphics queue
		VkSubmitInfo submit = {};
		submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit.pNext = nullptr;

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		submit.pWaitDstStageMask = &waitStage;

		submit.waitSemaphoreCount = 1;
		submit.pWaitSemaphores = &currentFramebuffer->m_PresentSemaphore;

		submit.signalSemaphoreCount = 1;
		submit.pSignalSemaphores = &currentFramebuffer->m_RenderSemaphore;

		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &m_FrameCommandBuffers[m_CurrentFramebuffer].buffer;

		CHECK_VULKAN(vkQueueSubmit(device->GetGraphicsQueue(), 1, &submit, currentFramebuffer->m_RenderFence));

		//present an image

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pWaitSemaphores = &currentFramebuffer->m_RenderSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &swapchainImageIndex;
		CHECK_VULKAN(vkQueuePresentKHR(device->GetGraphicsQueue(), &presentInfo));

		++m_CurrentFramebuffer;

		m_CurrentFramebuffer = m_CurrentFramebuffer % 2;
	}
}
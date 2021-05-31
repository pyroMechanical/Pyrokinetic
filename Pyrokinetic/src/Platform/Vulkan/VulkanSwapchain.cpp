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

	void VulkanSwapchain::CreateSwapchain(VkExtent2D extent)
	{
		PROFILE_FUNCTION();

		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

		m_Extent = extent;

		BuildSwapchain();

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.pNext = nullptr;
		fenceInfo.flags = 0;

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;
		m_RenderFences.resize(m_SwapchainImageViews.size());
		m_RenderSemaphores.resize(m_SwapchainImageViews.size());
		m_PresentSemaphores.resize(m_SwapchainImageViews.size());

		for (int i = 0; i < m_RenderFences.size(); i++)
		{
			CHECK_VULKAN(vkCreateFence(device, &fenceInfo, nullptr, &m_RenderFences[i]));
			CHECK_VULKAN(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_PresentSemaphores[i]));
			CHECK_VULKAN(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_RenderSemaphores[i]));
		}
	}

	void VulkanSwapchain::CreateFramebuffers(std::shared_ptr<RenderPass> renderPass)
	{
		std::shared_ptr<VulkanDevice> device = VulkanContext::Get()->GetDevice();

		m_RenderPass = std::dynamic_pointer_cast<VulkanRenderPass>(renderPass);

		VkRenderPass vkRenderPass = m_RenderPass->GetVulkanRenderPass();

		m_Framebuffers.resize(m_SwapchainImageViews.size());
		for (size_t i = 0; i < m_Framebuffers.size(); i++)
		{
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.pNext = nullptr;
			framebufferInfo.renderPass = vkRenderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.width = m_Extent.width;
			framebufferInfo.height = m_Extent.height;
			framebufferInfo.layers = 1;
			framebufferInfo.pAttachments = &m_SwapchainImageViews[i];

			m_FrameCommandBuffers.push_back(device->GetCommandBuffer(true, false));
			device->EndCommandBuffer(m_FrameCommandBuffers[i], false);
			CHECK_VULKAN(vkCreateFramebuffer(device->GetVulkanDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]))
		}
	}

	void VulkanSwapchain::RebuildSwapchain(VkExtent2D extent)
	{
		std::shared_ptr<VulkanDevice> device = VulkanContext::Get()->GetDevice();

		m_Extent = extent;

		for (auto& cmd : m_FrameCommandBuffers)
		{
			device->FlushCommandBuffer(cmd);
		}

		m_FrameCommandBuffers.clear();

		for(auto& framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(device->GetVulkanDevice(), framebuffer, nullptr);
		}

		m_Framebuffers.clear();

		vkb::destroy_swapchain(m_Swapchain);
	
		m_SwapchainImages.clear();
		m_SwapchainImageViews.clear();

		BuildSwapchain();

		CreateFramebuffers(std::dynamic_pointer_cast<RenderPass>(m_RenderPass));
	}

	void VulkanSwapchain::BuildSwapchain()
	{
		std::shared_ptr<VulkanDevice> device = VulkanContext::Get()->GetDevice();
		VkPhysicalDevice _physicalDevice = device->GetPhysicalDevice()->GetVulkanPhysicalDevice();
		vkb::SwapchainBuilder builder{ _physicalDevice, device->GetVulkanDevice(), m_Surface };

		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		m_Extent = { (uint32_t)width, (uint32_t)height };

		vkb::Swapchain vkb_swapchain = builder.set_desired_format({ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
			.set_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) //change this in future to change with VSync!
			.set_desired_extent(m_Extent.width, m_Extent.height)
			.build()
			.value();

		m_Swapchain = vkb_swapchain;
		m_SwapchainImageFormat = vkb_swapchain.image_format;
		m_SwapchainImages = vkb_swapchain.get_images().value();
		m_SwapchainImageViews = vkb_swapchain.get_image_views().value();
	}

	void VulkanSwapchain::SwapBuffers()
	{
		PROFILE_FUNCTION();

		std::shared_ptr<VulkanDevice> device = VulkanContext::Get()->GetDevice();

		uint32_t swapchainImageIndex = m_CurrentFramebuffer;
		//wait for rendering to complete
		CHECK_VULKAN(vkWaitForFences(device->GetVulkanDevice(), 1, &m_RenderFences[m_CurrentFramebuffer], VK_TRUE, 1000000000));
		vkResetFences(device->GetVulkanDevice(), 1, &m_RenderFences[m_CurrentFramebuffer]);

		CHECK_VULKAN(vkAcquireNextImageKHR(device->GetVulkanDevice(), m_Swapchain.swapchain, 1000000000, m_PresentSemaphores[m_CurrentFramebuffer], nullptr, &swapchainImageIndex));

		//submit current frame's command buffer to the graphics queue
		VkSubmitInfo submit = {};
		submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit.pNext = nullptr;

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		submit.pWaitDstStageMask = &waitStage;

		submit.waitSemaphoreCount = 1;
		submit.pWaitSemaphores = &m_PresentSemaphores[m_CurrentFramebuffer];

		submit.signalSemaphoreCount = 1;
		submit.pSignalSemaphores = &m_RenderSemaphores[m_CurrentFramebuffer];

		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &m_FrameCommandBuffers[m_CurrentFramebuffer].buffer;

		CHECK_VULKAN(vkQueueSubmit(device->GetGraphicsQueue(), 1, &submit, m_RenderFences[m_CurrentFramebuffer]));

		//present an image

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_Swapchain.swapchain;
		presentInfo.pWaitSemaphores = &m_RenderSemaphores[m_CurrentFramebuffer];
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &swapchainImageIndex;
		CHECK_VULKAN(vkQueuePresentKHR(device->GetGraphicsQueue(), &presentInfo));

		++m_CurrentFramebuffer;

		m_CurrentFramebuffer = m_CurrentFramebuffer % 2;
	}
}
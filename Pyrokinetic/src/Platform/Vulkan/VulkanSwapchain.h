#pragma once
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"
#include "VkBootstrap.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace pk
{
	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(GLFWwindow* window)
			: m_Window(window) {}
		void CreateSurface();
		void CreateSwapchain(VkExtent2D extent);
		void CreateFramebuffers(std::shared_ptr<RenderPass> renderPass);
		void RebuildSwapchain(VkExtent2D extent);
		void SwapBuffers();

		const VkSurfaceKHR& GetVulkanSurface() const { return m_Surface; }

		uint32_t GetWidth() { return m_Extent.width; }
		uint32_t GetHeight() { return m_Extent.height; }
		VkExtent2D GetExtent() { return m_Extent; }
		VkFormat GetSwapchainImageFormat() { return m_SwapchainImageFormat; }

		VkRenderPass GetRenderPass() { return m_RenderPass->GetVulkanRenderPass(); }
		VkClearValue GetClearValue() { return m_ClearValue; }
		VkFramebuffer GetCurrentFramebuffer() { return m_Framebuffers[m_CurrentFramebuffer]; }
		std::vector<VkFramebuffer>& GetFramebuffers() { return m_Framebuffers; }
		VulkanDevice::VulkanCommandBuffer& GetCurrentCommandBuffer() { return m_FrameCommandBuffers[m_CurrentFramebuffer]; }
		std::vector<VulkanDevice::VulkanCommandBuffer>& GetCommandBuffers() { return m_FrameCommandBuffers; }

	private:
		void BuildSwapchain();
	private:
		GLFWwindow* m_Window = nullptr;
		VkSurfaceKHR m_Surface = nullptr;
		vkb::Swapchain m_Swapchain;

		std::shared_ptr<VulkanRenderPass> m_RenderPass = nullptr;
		VkClearValue m_ClearValue;

		VkExtent2D m_Extent;

		VkFormat m_SwapchainImageFormat = VK_FORMAT_UNDEFINED;
		uint32_t m_CurrentFramebuffer = 0;
		std::vector<VkFramebuffer> m_Framebuffers;
		std::vector<VkFence> m_RenderFences;
		std::vector<VkSemaphore> m_PresentSemaphores;
		std::vector<VkSemaphore> m_RenderSemaphores;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
		std::vector<VulkanDevice::VulkanCommandBuffer> m_FrameCommandBuffers;
	};
}
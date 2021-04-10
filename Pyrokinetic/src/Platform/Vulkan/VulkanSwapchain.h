#pragma once
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"
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
		void CreateSwapchain(const std::shared_ptr<VulkanDevice>& device);
		void CreateFramebuffers(const FramebufferSpecification& spec);

		const VkSurfaceKHR& GetVulkanSurface() const { return m_Surface; }

		uint32_t GetWidth() { return m_Extent.width; }
		uint32_t GetHeight() { return m_Extent.height; }
		VkExtent2D GetExtent() { return m_Extent; }
		VkFormat GetSwapchainImageFormat() { return m_SwapchainImageFormat; }

		VulkanRenderPass* GetRenderPass() { return m_RenderPass.get(); }
		VkClearValue GetClearValue() { return m_ClearValue; }
		VulkanFramebuffer GetCurrentFramebuffer() { return m_Framebuffers[m_CurrentFramebuffer]; }
		std::vector<VulkanFramebuffer>& GetFramebuffers() { return m_Framebuffers; }
		std::vector<VulkanDevice::VulkanCommandBuffer> GetCommandBuffers() { return m_FrameCommandBuffers; }

	private:
		GLFWwindow* m_Window = nullptr;
		VkSurfaceKHR m_Surface = nullptr;
		VkSwapchainKHR m_Swapchain;

		std::shared_ptr<VulkanRenderPass> m_RenderPass = nullptr;
		VkClearValue m_ClearValue;

		VkExtent2D m_Extent;

		VkFormat m_SwapchainImageFormat = VK_FORMAT_UNDEFINED;
		uint32_t m_CurrentFramebuffer = 0;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
		std::vector<VulkanFramebuffer> m_Framebuffers;
		std::vector<VulkanDevice::VulkanCommandBuffer> m_FrameCommandBuffers;
	};
}
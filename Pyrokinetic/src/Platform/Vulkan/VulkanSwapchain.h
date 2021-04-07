#pragma once
#include "VulkanFramebuffer.h"
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

		const VkSurfaceKHR& GetVulkanSurface() const { return m_Surface; }

	private:
		GLFWwindow* m_Window = nullptr;
		VkSurfaceKHR m_Surface = nullptr;
		VkSwapchainKHR m_Swapchain;
		VkFormat m_SwapchainImageFormat = VK_FORMAT_UNDEFINED;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
	};
}
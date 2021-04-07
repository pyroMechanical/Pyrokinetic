#include "pkpch.h"
#include "VulkanSwapchain.h"
#include "VulkanContext.h"

namespace pk
{
	void VulkanSwapchain::CreateSurface()
	{
		glfwCreateWindowSurface(VulkanContext::GetVulkanInstance(), m_Window, nullptr, &m_Surface);
	}

	void VulkanSwapchain::CreateSwapchain(const std::shared_ptr<VulkanDevice>& device)
	{
		VkDevice _device = device->GetVulkanDevice();
		VkPhysicalDevice _physicalDevice = device->GetPhysicalDevice()->GetVulkanPhysicalDevice();
		vkb::SwapchainBuilder builder{ _physicalDevice, _device, m_Surface };

		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);

		vkb::Swapchain vkb_swapchain = builder.use_default_format_selection()
			.set_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) //change this in future to change with VSync!
			.set_desired_extent((uint32_t)width, (uint32_t)height)
			.build()
			.value();
	}
}
#include "pkpch.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"

namespace pk
{
	VulkanPhysicalDevice::VulkanPhysicalDevice(VkSurfaceKHR surface)
	{
		auto vkInstance = VulkanContext::GetVkbInstance();

		vkb::PhysicalDeviceSelector selector{ vkInstance };
		auto physicalDeviceResult = selector.set_minimum_version(1, 1)
										.set_desired_version(1, 2)
										.set_surface(surface)
										.select();
		m_PhysicalDevice = physicalDeviceResult.value();
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice()
	{
	}

	VulkanDevice::VulkanDevice(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice)
	{
		vkb::DeviceBuilder builder{ physicalDevice->m_PhysicalDevice };
		m_LogicalDevice = builder.build().value();
		m_PhysicalDevice = physicalDevice;

		m_GraphicsQueueFamily = m_LogicalDevice.get_queue_index(vkb::QueueType::graphics).value();
		m_ComputeQueueFamily = m_LogicalDevice.get_queue_index(vkb::QueueType::compute).value();

		m_GraphicsQueue = m_LogicalDevice.get_queue(vkb::QueueType::graphics).value();
		m_ComputeQueue = m_LogicalDevice.get_queue(vkb::QueueType::compute).value();

	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(m_LogicalDevice.device, nullptr);
	}
}
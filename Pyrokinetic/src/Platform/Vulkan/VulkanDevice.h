#pragma once
#include <vulkan/vulkan.h>
#include "VkBootstrap.h"

namespace pk
{
	class VulkanPhysicalDevice
	{
	public:
	public:
		VulkanPhysicalDevice(VkSurfaceKHR surface);
		~VulkanPhysicalDevice();

		VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_PhysicalDevice.physical_device; }
		
		VkFormat GetDepthFormat() const { return m_DepthFormat; }

	private:
		VkFormat FindDepthFormat() const;
	private:

		vkb::PhysicalDevice m_PhysicalDevice;

		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::vector<VkDeviceQueueCreateInfo > m_QueueCreateInfos;

		friend class VulkanDevice;
	};

	class VulkanDevice
	{
	public:
		struct VulkanCommandBuffer
		{
			VkCommandBuffer buffer;
			VkQueue& queue;
			VkCommandPool& pool;
		};
	public:
		VulkanDevice(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice);
		~VulkanDevice();
		
		uint32_t GetGraphicsQueueFamily() { return m_GraphicsQueueFamily; }
		uint32_t GetComputeQueueFamily() { return m_ComputeQueueFamily; }

		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
		VkQueue GetComputeQueue() { return m_ComputeQueue; }

		VulkanCommandBuffer GetCommandBuffer(bool begin, bool compute = false);
		
		void EndCommandBuffer(VulkanCommandBuffer& buffer, bool flush = false);

		void FlushCommandBuffer(VulkanCommandBuffer buffer);

		const std::shared_ptr<VulkanPhysicalDevice>& GetPhysicalDevice() const { return m_PhysicalDevice; }

		VkDevice GetVulkanDevice() const { return m_LogicalDevice.device; }
	private:
		vkb::Device m_LogicalDevice;
		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		VkPhysicalDeviceFeatures m_EnabledFeatures;
		VkCommandPool m_GraphicsCommandPool, m_ComputeCommandPool;

		uint32_t m_GraphicsQueueFamily, m_ComputeQueueFamily;
		VkQueue m_GraphicsQueue;
		VkQueue m_ComputeQueue;

		bool m_EnableDebugMarkers = true;
	};
}

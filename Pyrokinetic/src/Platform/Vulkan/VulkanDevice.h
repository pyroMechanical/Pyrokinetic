#pragma once
#include <vulkan/vulkan.h>

namespace pk
{
	class VulkanPhysicalDevice
	{
	public:
		struct QueueFamilyIndices
		{
			int32_t Graphics = -1;
			int32_t Compute = -1;
			int32_t Transfer = -1;
		};
	public:
		VulkanPhysicalDevice(VkSurfaceKHR surface);
		~VulkanPhysicalDevice();

		bool IsExtensionSupported(const std::string& extensionName) const;

		VkPhysicalDevice GetVulkanPhysicalDevice() const { return m_PhysicalDevice.physical_device; }
		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }

		VkFormat GetDepthFormat() const { return m_DepthFormat; }

	private:
		VkFormat FindDepthFormat() const;
		QueueFamilyIndices GetQueueFamilyIndices(int queueFlags);
	private:
		QueueFamilyIndices m_QueueFamilyIndices;

		vkb::PhysicalDevice m_PhysicalDevice;

		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::vector<VkDeviceQueueCreateInfo > m_QueueCreateInfos;

		friend class VulkanDevice;
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(const std::shared_ptr<VulkanPhysicalDevice>& physicalDevice);
		~VulkanDevice();
		
		uint32_t GetGraphicsQueueFamily() { return m_GraphicsQueueFamily; }
		uint32_t GetComputeQueueFamily() { return m_ComputeQueueFamily; }

		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
		VkQueue GetComputeQueue() { return m_ComputeQueue; }

		VkCommandBuffer GetCommandBuffer(bool begin, bool compute = false);
		

		const std::shared_ptr<VulkanPhysicalDevice>& GetPhysicalDevice() const { return m_PhysicalDevice; }

		VkDevice GetVulkanDevice() const { return m_LogicalDevice.device; }
	private:
		vkb::Device m_LogicalDevice;
		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		VkPhysicalDeviceFeatures m_EnabledFeatures;
		VkCommandPool m_RenderCommandPool, m_ComputeCommandPool, m_ImmediateCommandPool;

		uint32_t m_GraphicsQueueFamily, m_ComputeQueueFamily;
		VkQueue m_GraphicsQueue;
		VkQueue m_ComputeQueue;

		bool m_EnableDebugMarkers = true;
	};
}

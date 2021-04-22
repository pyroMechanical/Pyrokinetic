#include "pkpch.h"
#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "VulkanInitializer.h"

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

		auto graphicsPoolCreateInfo = vkinit::command_pool_create_info(m_GraphicsQueueFamily, 0);

		CHECK_VULKAN(vkCreateCommandPool(m_LogicalDevice.device, &graphicsPoolCreateInfo, nullptr, &m_GraphicsCommandPool));

		auto computePoolCreateInfo = vkinit::command_pool_create_info(m_ComputeQueueFamily, 0);

		CHECK_VULKAN(vkCreateCommandPool(m_LogicalDevice.device, &computePoolCreateInfo, nullptr, &m_ComputeCommandPool));
	}

	VulkanDevice::~VulkanDevice()
	{
	}

	VulkanDevice::VulkanCommandBuffer VulkanDevice::GetCommandBuffer(bool begin, bool compute)
	{
		VkCommandBuffer buffer;

		VkCommandPool& pool = compute ? m_ComputeCommandPool : m_GraphicsCommandPool;
		VkQueue& queue = compute ? m_ComputeQueue : m_GraphicsQueue;

		auto bufferAllocateInfo = vkinit::command_buffer_allocate_info(pool);

		CHECK_VULKAN(vkAllocateCommandBuffers(m_LogicalDevice.device, &bufferAllocateInfo, &buffer));

		if (begin)
		{
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			CHECK_VULKAN(vkBeginCommandBuffer(buffer, &info));
		}

		VulkanCommandBuffer bufferWrap{ buffer, queue, pool };
		return bufferWrap;
	}

	void VulkanDevice::EndCommandBuffer(VulkanDevice::VulkanCommandBuffer buffer, bool flush)
	{
		PK_CORE_ASSERT(buffer.buffer != VK_NULL_HANDLE, "Buffer does not exist!");

		CHECK_VULKAN(vkEndCommandBuffer(buffer.buffer));

		if(flush)
		{
			FlushCommandBuffer(buffer);
		}
	}

	void VulkanDevice::FlushCommandBuffer(VulkanDevice::VulkanCommandBuffer buffer)
	{
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &buffer.buffer;

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = 0;
		VkFence fence;
		CHECK_VULKAN(vkCreateFence(m_LogicalDevice.device, &fenceCreateInfo, nullptr, &fence));

		CHECK_VULKAN(vkQueueSubmit(buffer.queue, 1, &submitInfo, fence));

		CHECK_VULKAN(vkWaitForFences(m_LogicalDevice.device, 1, &fence, VK_TRUE, 10000000));

		vkDestroyFence(m_LogicalDevice.device, fence, nullptr);

		vkFreeCommandBuffers(m_LogicalDevice.device, buffer.pool, 1, &buffer.buffer);
	}
}
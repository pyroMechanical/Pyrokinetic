#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace pk {
	struct AllocatedBuffer
	{
		VkBuffer buffer;
		VmaAllocation allocation;
	};

	struct FrameData {
		VkSemaphore presentSemaphore, renderSemaphore;
		VkFence renderFence;

		VkCommandPool commandPool;
		VkCommandBuffer mainCommandBuffer;
	};
}
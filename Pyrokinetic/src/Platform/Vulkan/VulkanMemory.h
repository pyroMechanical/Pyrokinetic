#pragma once
#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace pk
{
	class VulkanMemory
	{
	public:
		static std::shared_ptr<VulkanMemory> GetInstance() { return s_Instance; }
		VmaAllocator* GetAllocator() { return &allocator; }

	private:
		VmaAllocator allocator;
		static std::shared_ptr<VulkanMemory> s_Instance;
	};
	
}
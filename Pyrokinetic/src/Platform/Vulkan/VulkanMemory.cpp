#include "pkpch.h"
#include "VulkanMemory.h"

namespace pk
{
	std::shared_ptr<VulkanMemory> VulkanMemory::s_Instance = std::make_shared<VulkanMemory>();
}
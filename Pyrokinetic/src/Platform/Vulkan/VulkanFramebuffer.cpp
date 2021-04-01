#include "pkpch.h"
#include "VulkanFramebuffer.h"

namespace pk
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
	}
}
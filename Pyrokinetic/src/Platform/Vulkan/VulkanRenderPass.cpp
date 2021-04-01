#include "pkpch.h"
#include "VulkanRenderPass.h"

namespace pk
{
	VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification& spec)
		: m_Spec(spec)
	{
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
	}
}
#include "pkpch.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"

namespace pk
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->AddFramebuffer(this);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->RemoveFramebuffer(this);
	}
}
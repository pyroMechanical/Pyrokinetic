#pragma once

#include "Pyrokinetic/Rendering/RenderPass.h"
#include <vulkan/vulkan.h>

namespace pk
{
	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(const RenderPassSpecification& spec);
		virtual ~VulkanRenderPass() override;

		virtual RenderPassSpecification GetSpecification() const override { return m_Spec; }
	private:
		RenderPassSpecification m_Spec;
		VkRenderPass m_RenderPass;
	};
}
#pragma once

#include "Pyrokinetic/Rendering/RenderPass.h"
#include "VulkanFramebuffer.h"
#include <vulkan/vulkan.h>

namespace pk
{
	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(const RenderPassSpecification& spec);
		virtual ~VulkanRenderPass() override;

		virtual RenderPassSpecification GetSpecification() const override { return m_Spec; }

		std::unordered_set<VulkanFramebuffer*> GetFramebuffers() { return m_Framebuffers; }

		void AddFramebuffer(VulkanFramebuffer* framebuffer) { m_Framebuffers.emplace(framebuffer); }

		void RemoveFramebuffer(VulkanFramebuffer* framebuffer) { m_Framebuffers.erase(framebuffer); }

		VkRenderPass GetVulkanRenderPass() { return m_RenderPass; }
	private:
		RenderPassSpecification m_Spec;
		VkRenderPass m_RenderPass;
		VkFormat m_Format;
		std::unordered_set<VulkanFramebuffer*> m_Framebuffers;
		std::vector<FramebufferTextureSpecification> m_ColorSpecifications;
		FramebufferTextureSpecification m_DepthSpecification;
	};
}
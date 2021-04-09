#pragma once

#include "Pyrokinetic/Rendering/Framebuffer.h"
#include <vulkan/vulkan.h>

namespace pk
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer() override;

		virtual void Bind() override {};
		virtual void Unbind() override {};

		VkFramebuffer GetVulkanFramebuffer() { return m_Framebuffer; }

		const VkFramebuffer GetVulkanFramebuffer() const { return m_Framebuffer; }

		virtual void Resize(const uint32_t width, const uint32_t height) override {};

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { return 0; };

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Spec; }

	private:
		uint32_t m_RendererID;
		FramebufferSpecification m_Spec;
		VkFramebuffer m_Framebuffer;
	};
}
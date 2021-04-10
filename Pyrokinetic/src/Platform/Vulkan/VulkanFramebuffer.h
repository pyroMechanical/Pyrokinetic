#pragma once

#include "Pyrokinetic/Rendering/Framebuffer.h"
#include <vulkan/vulkan.h>

namespace pk
{
	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		VulkanFramebuffer(const FramebufferSpecification& spec, VkImage image, VkImageView imageView);
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
		VkImage m_Image;
		VkImageView m_ImageView;
		VkFramebuffer m_Framebuffer;
	};
}
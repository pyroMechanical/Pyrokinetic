#pragma once

#include "Pyrokinetic/Rendering/Framebuffer.h"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include "backends/imgui_impl_vulkan.h"

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

		virtual void* GetColorAttachment(uint32_t index = 0) const override { return (void*)ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); };

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Spec; }

	private:
		uint32_t m_RendererID;
		FramebufferSpecification m_Spec;
		VkImage m_Image;
		VmaAllocation m_ImageAllocation = nullptr;
		VkImageView m_ImageView;
		VkFramebuffer m_Framebuffer;
		VkSampler m_Sampler;
		VkSemaphore m_RenderSemaphore, m_PresentSemaphore;
		VkFence m_RenderFence;
		VkDescriptorSet descriptorSet;
		friend class VulkanSwapchain;
	};
}
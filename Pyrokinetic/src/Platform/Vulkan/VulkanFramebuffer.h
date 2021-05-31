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
		virtual ~VulkanFramebuffer() override;

		virtual void Bind() override {};
		virtual void Unbind() override {};

		void CreateAttachment(FramebufferTextureSpecification attachmentSpec);

		VkFramebuffer GetVulkanFramebuffer() { return m_Framebuffer; }

		const VkFramebuffer GetVulkanFramebuffer() const { return m_Framebuffer; }

		void DestroyFramebufferImages();

		virtual void Resize(const uint32_t width, const uint32_t height) override;

		virtual void* GetColorAttachment(uint32_t index = 0) const override { PK_CORE_ASSERT(index < m_ImTextureIDs.size(), "framebuffer attachment index out of bounds!"); return m_ImTextureIDs[index]; };

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Spec; }

	private:
		uint32_t m_RendererID;
		FramebufferSpecification m_Spec;

		std::vector<FramebufferTextureSpecification> m_AttachmentSpecifications;

		std::vector<VkImage> m_Images;
		std::vector<VmaAllocation> m_ImageAllocations;
		std::vector<VkImageView> m_ImageViews;
		VkFramebuffer m_Framebuffer;
		std::vector<VkSampler> m_Samplers;
		VkSemaphore m_RenderSemaphore, m_PresentSemaphore;
		VkFence m_RenderFence;

		VkDescriptorPool m_Pool;

		std::vector<void*> m_ImTextureIDs;
		friend class VulkanSwapchain;
	};
}
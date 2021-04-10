#include "pkpch.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"

namespace pk
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->AddFramebuffer(this);

		/*VulkanRenderPass vkRenderPass = *dynamic_cast<VulkanRenderPass*>(spec.renderPass.get());

		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.pNext = nullptr;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.renderPass = vkRenderPass.GetVulkanRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &m_ImageView;
		framebufferInfo.width = spec.width;
		framebufferInfo.height = spec.height;
		framebufferInfo.layers = 1;

		CHECK_VULKAN(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_Framebuffer));*/
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec, VkImage image, VkImageView imageView)
		: m_Spec(spec), m_Image(image), m_ImageView(imageView)
	{
		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->AddFramebuffer(this);

		VulkanRenderPass vkRenderPass = *dynamic_cast<VulkanRenderPass*>(spec.renderPass.get());

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.renderPass = vkRenderPass.GetVulkanRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &m_ImageView;
		framebufferInfo.width = spec.width;
		framebufferInfo.height = spec.height;
		framebufferInfo.layers = 1;

		CHECK_VULKAN(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_Framebuffer));
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->RemoveFramebuffer(this);
		vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
	}
}
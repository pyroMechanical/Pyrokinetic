#include "pkpch.h"
#include "VulkanFramebuffer.h"
#include "VulkanInitializer.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"
#include <memory>
#include "stb_image.h"

namespace pk
{
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{

		VulkanContext* context = VulkanContext::Get();
		std::shared_ptr<VulkanDevice> device = context->GetDevice();
		VmaAllocator* allocator = context->GetAllocator();

		const uint32_t width = spec.width;
		const uint32_t height = spec.height;

		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->AddFramebuffer(this);

		VulkanRenderPass* vkRenderPass = dynamic_cast<VulkanRenderPass*>(spec.renderPass.get());

		std::vector<uint8_t> imageData(spec.width * spec.height * 4, 0x00);

		VkDeviceSize imageSize = imageData.size();

		VkFormat image_format = VK_FORMAT_B8G8R8A8_UNORM;

		AllocatedBuffer stagingBuffer = vkinit::allocate_buffer(*allocator, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void* data;
		vmaMapMemory(*allocator, stagingBuffer.allocation, &data);
		memcpy(data, imageData.data(), imageSize);
		vmaUnmapMemory(*allocator, stagingBuffer.allocation);

		imageData.clear();

		VkExtent3D imageExtent;
		imageExtent.width = spec.width;
		imageExtent.height = spec.height;
		imageExtent.depth = 1;

		VkImageCreateInfo image_info = vkinit::image_create_info(image_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, imageExtent);

		VmaAllocationCreateInfo image_allocinfo = {};
		image_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		CHECK_VULKAN(vmaCreateImage(*allocator, &image_info, &image_allocinfo, &m_Image, &m_ImageAllocation, nullptr));

		auto cmd = device->GetCommandBuffer(true);
		VkImageSubresourceRange range;
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		VkImageMemoryBarrier imageBarrier_toTransfer = {};
		imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toTransfer.image = m_Image;
		imageBarrier_toTransfer.subresourceRange = range;

		imageBarrier_toTransfer.srcAccessMask = 0;
		imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(cmd.buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = imageExtent;

		vkCmdCopyBufferToImage(cmd.buffer, stagingBuffer.buffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

		imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd.buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);
		vmaDestroyBuffer(*allocator, stagingBuffer.buffer, stagingBuffer.allocation);

		VkImageViewCreateInfo imageViewInfo = vkinit::imageview_create_info(image_format, m_Image, VK_IMAGE_ASPECT_COLOR_BIT);
		vkCreateImageView(device->GetVulkanDevice(), &imageViewInfo, nullptr, &m_ImageView);

		VkSamplerCreateInfo samplerInfo = vkinit::image_sampler_create_info(VK_FILTER_NEAREST);

		vkCreateSampler(device->GetVulkanDevice(), &samplerInfo, nullptr, &m_Sampler);

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.renderPass = vkRenderPass->GetVulkanRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.width = spec.width;
		framebufferInfo.height = spec.height;
		framebufferInfo.layers = 1;

		framebufferInfo.pAttachments = &m_ImageView;
		CHECK_VULKAN(vkCreateFramebuffer(device->GetVulkanDevice(), &framebufferInfo, nullptr, &m_Framebuffer));

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		CHECK_VULKAN(vkCreateFence(device->GetVulkanDevice(), &fenceCreateInfo, nullptr, &m_RenderFence));

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		CHECK_VULKAN(vkCreateSemaphore(device->GetVulkanDevice(), &semaphoreCreateInfo, nullptr, &m_RenderSemaphore));
		CHECK_VULKAN(vkCreateSemaphore(device->GetVulkanDevice(), &semaphoreCreateInfo, nullptr, &m_PresentSemaphore));

		AllocatedBuffer cameraMatrix = vkinit::allocate_buffer(*allocator, sizeof(glm::mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec, VkImage image, VkImageView imageView)
		: m_Spec(spec), m_Image(image), m_ImageView(imageView)
	{
		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->AddFramebuffer(this);

		VulkanRenderPass* vkRenderPass = dynamic_cast<VulkanRenderPass*>(spec.renderPass.get());

		VkSamplerCreateInfo samplerInfo = vkinit::image_sampler_create_info(VK_FILTER_NEAREST);

		vkCreateSampler(device, &samplerInfo, nullptr, &m_Sampler);

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.renderPass = vkRenderPass->GetVulkanRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.width = spec.width;
		framebufferInfo.height = spec.height;
		framebufferInfo.layers = 1;

		framebufferInfo.pAttachments = &m_ImageView;
		CHECK_VULKAN(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_Framebuffer));

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		CHECK_VULKAN(vkCreateFence(device, &fenceCreateInfo, nullptr, &m_RenderFence));

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		CHECK_VULKAN(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_RenderSemaphore));
		CHECK_VULKAN(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_PresentSemaphore));
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		VulkanContext* context = VulkanContext::Get();

		VkDevice device = context->GetDevice()->GetVulkanDevice();
		vkWaitForFences(device, 1, &m_RenderFence, true, 1000000000);
		vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
		vkDestroyImageView(device, m_ImageView, nullptr);
		if (m_ImageAllocation == nullptr)
		{
			vkDestroyImage(device, m_Image, nullptr);
		}
		else
		{
			vmaDestroyImage(*context->GetAllocator(), m_Image, m_ImageAllocation);
		}
		vkDestroyFence(device, m_RenderFence, nullptr);
		vkDestroySemaphore(device, m_RenderSemaphore, nullptr);
		vkDestroySemaphore(device, m_PresentSemaphore, nullptr);
		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->RemoveFramebuffer(this);
	}
}
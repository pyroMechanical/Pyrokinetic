#include "pkpch.h"
#include "VulkanFramebuffer.h"
#include "VulkanInitializer.h"
#include "VulkanRenderPass.h"
#include "VulkanContext.h"
#include <memory>
#include "stb_image.h"

namespace pk
{
	namespace util
	{
		static bool IsDepthFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::DEPTH24STENCIL8: return true;
			case ImageFormat::DEPTH32: return true;
			}

			return false;
		}

		static VkFormat GetVulkanFormatFromImageFormat(ImageFormat format)
		{
			switch(format)
			{
			case ImageFormat::RGBA8: return VK_FORMAT_B8G8R8A8_UNORM;
			case ImageFormat::DEPTH32: return VK_FORMAT_D32_SFLOAT;
			case ImageFormat::DEPTH24STENCIL8: return VK_FORMAT_D32_SFLOAT;
			}
		}

		uint32_t VulkanFormatBitSize(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::RGBA8: return 4;
				case ImageFormat::DEPTH32: return 4;
				case ImageFormat::DEPTH24STENCIL8: return 4;
			}
		}
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec)
		: m_Spec(spec)
	{
		PROFILE_FUNCTION();
		VulkanContext* context = VulkanContext::Get();
		std::shared_ptr<VulkanDevice> device = context->GetDevice();
		VmaAllocator* allocator = context->GetAllocator();

		const uint32_t width = spec.width;
		const uint32_t height = spec.height;

		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->AddFramebuffer(this);

		VulkanRenderPass* vkRenderPass = dynamic_cast<VulkanRenderPass*>(spec.renderPass.get());

		m_AttachmentSpecifications = m_Spec.renderPass->GetSpecification().Attachments.Attachments;

		for(auto& attachmentSpec : m_AttachmentSpecifications)
		{
			CreateAttachment(attachmentSpec);
		}

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.renderPass = vkRenderPass->GetVulkanRenderPass();
		framebufferInfo.attachmentCount = m_ImageViews.size();
		framebufferInfo.width = spec.width;
		framebufferInfo.height = spec.height;
		framebufferInfo.layers = 1;
		framebufferInfo.pAttachments = m_ImageViews.data();

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
	}

	void VulkanFramebuffer::CreateAttachment(FramebufferTextureSpecification attachmentSpec)
	{
		VulkanContext* context = VulkanContext::Get();
		std::shared_ptr<VulkanDevice> device = context->GetDevice();
		VmaAllocator* allocator = context->GetAllocator();

		std::vector<uint8_t> imageData(m_Spec.width * m_Spec.height * util::VulkanFormatBitSize(attachmentSpec.TextureFormat), 0x00);

		VkDeviceSize imageSize = imageData.size();

		VkFormat image_format = util::GetVulkanFormatFromImageFormat(attachmentSpec.TextureFormat);

		AllocatedBuffer stagingBuffer = vkinit::allocate_buffer(*allocator, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

		void* data;
		vmaMapMemory(*allocator, stagingBuffer.allocation, &data);
		memcpy(data, imageData.data(), imageSize);
		vmaUnmapMemory(*allocator, stagingBuffer.allocation);

		imageData.clear();

		VkExtent3D imageExtent;
		imageExtent.width = m_Spec.width;
		imageExtent.height = m_Spec.height;
		imageExtent.depth = 1;

		VkImageCreateInfo image_info = vkinit::image_create_info(image_format, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | (util::IsDepthFormat(attachmentSpec.TextureFormat) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT), imageExtent);

		VmaAllocationCreateInfo image_allocinfo = {};
		image_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkImage& image = m_Images.emplace_back();
		VmaAllocation& imageAlloc = m_ImageAllocations.emplace_back();
		VkImageView& imageView = m_ImageViews.emplace_back();
		VkSampler& sampler = m_Samplers.emplace_back();

		CHECK_VULKAN(vmaCreateImage(*allocator, &image_info, &image_allocinfo, &image, &imageAlloc, nullptr));

		auto cmd = device->GetCommandBuffer(true);
		VkImageSubresourceRange range;
		range.aspectMask = (util::IsDepthFormat(attachmentSpec.TextureFormat) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		VkImageMemoryBarrier imageBarrier_toTransfer = {};
		imageBarrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		imageBarrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toTransfer.image = image;
		imageBarrier_toTransfer.subresourceRange = range;

		imageBarrier_toTransfer.srcAccessMask = 0;
		imageBarrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(cmd.buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toTransfer);

		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = util::IsDepthFormat(attachmentSpec.TextureFormat) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = imageExtent;

		vkCmdCopyBufferToImage(cmd.buffer, stagingBuffer.buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		VkImageMemoryBarrier imageBarrier_toReadable = imageBarrier_toTransfer;

		imageBarrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageBarrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageBarrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd.buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageBarrier_toReadable);

		device->EndCommandBuffer(cmd, true);
		vmaDestroyBuffer(*allocator, stagingBuffer.buffer, stagingBuffer.allocation);

		VkImageViewCreateInfo imageViewInfo = vkinit::imageview_create_info(image_format, image, util::IsDepthFormat(attachmentSpec.TextureFormat) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);
		vkCreateImageView(device->GetVulkanDevice(), &imageViewInfo, nullptr, &imageView);

		VkSamplerCreateInfo samplerInfo = vkinit::image_sampler_create_info(VK_FILTER_NEAREST);

		vkCreateSampler(device->GetVulkanDevice(), &samplerInfo, nullptr, &sampler);

		m_ImTextureIDs.emplace_back((void*)ImGui_ImplVulkan_AddTexture(sampler, imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec, VkImage image, VkImageView imageView)
		: m_Spec(spec)
	{
		PROFILE_FUNCTION();
		VkDevice device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

		m_Images.emplace_back(image);
		m_ImageViews.emplace_back(imageView);

		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->AddFramebuffer(this);

		VulkanRenderPass* vkRenderPass = dynamic_cast<VulkanRenderPass*>(spec.renderPass.get());

		VkSamplerCreateInfo samplerInfo = vkinit::image_sampler_create_info(VK_FILTER_NEAREST);

		vkCreateSampler(device, &samplerInfo, nullptr, &m_Samplers.emplace_back());

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.renderPass = vkRenderPass->GetVulkanRenderPass();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.width = spec.width;
		framebufferInfo.height = spec.height;
		framebufferInfo.layers = 1;

		framebufferInfo.pAttachments = &imageView;
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
	void VulkanFramebuffer::Resize(const uint32_t width, const uint32_t height)
	{
		
		//DestroyFramebufferImages();
	}

	void VulkanFramebuffer::DestroyFramebufferImages()
	{
		VulkanContext* context = VulkanContext::Get();

		VkDevice device = context->GetDevice()->GetVulkanDevice();
		vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
		for (auto& imageView : m_ImageViews)
			vkDestroyImageView(device, imageView, nullptr);
		if (m_ImageAllocations.size() == 0)
		{
			for (auto& image : m_Images)
				vkDestroyImage(device, image, nullptr);
		}
		else
		{
			for (size_t i = 0; i < m_Images.size(); i++)
				vmaDestroyImage(*context->GetAllocator(), m_Images[i], m_ImageAllocations[i]);
		}
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		PROFILE_FUNCTION();
		VulkanContext* context = VulkanContext::Get();

		VkDevice device = context->GetDevice()->GetVulkanDevice();
		vkWaitForFences(device, 1, &m_RenderFence, true, 1000000000);
		DestroyFramebufferImages();
		vkDestroyFence(device, m_RenderFence, nullptr);
		vkDestroySemaphore(device, m_RenderSemaphore, nullptr);
		vkDestroySemaphore(device, m_PresentSemaphore, nullptr);
		dynamic_cast<VulkanRenderPass*>(m_Spec.renderPass.get())->RemoveFramebuffer(this);
	}
}
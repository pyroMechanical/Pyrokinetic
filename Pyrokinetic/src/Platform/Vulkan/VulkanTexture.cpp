#include "pkpch.h"
#include "VulkanTexture.h"
#include "VulkanInitializer.h"
#include "VulkanContext.h"
#include <vulkan/vulkan.h>

#include "stb_image.h"

namespace pk
{
	VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		PROFILE_FUNCTION();
		VulkanContext* context = VulkanContext::Get();

		VmaAllocator* allocator = context->GetAllocator();

		auto device = context->GetDevice();

		VkFormat internalFormat = VK_FORMAT_B8G8R8A8_UNORM;

		m_InternalFormat = internalFormat;

		m_Width = width;
		m_Height = height;
		VkDeviceSize imageSize = width * height * 4;

		std::vector<uint8_t> data;

		data.resize(imageSize);

		std::fill(data.begin(), data.end(), 0xff);

		PK_CORE_ASSERT(internalFormat, "Format not supported!");

		VkBufferCreateInfo stagingBufferInfo = {};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferInfo.size = imageSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo stagingBufferAllocInfo = {};
		stagingBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		AllocatedBuffer stagingBuffer;

		CHECK_VULKAN(vmaCreateBuffer(*allocator, &stagingBufferInfo, &stagingBufferAllocInfo, &stagingBuffer.buffer, &stagingBuffer.allocation, nullptr));

		void* region;

		vmaMapMemory(*allocator, stagingBuffer.allocation, &region);
		memcpy(region, data.data(), static_cast<size_t>(imageSize));
		vmaUnmapMemory(*allocator, stagingBuffer.allocation);

		VkExtent3D imageExtent;
		imageExtent.width = static_cast<uint32_t>(m_Width);
		imageExtent.height = static_cast<uint32_t>(m_Height);
		imageExtent.depth = 1;

		VkImageCreateInfo imgInfo = vkinit::image_create_info(internalFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

		VmaAllocationCreateInfo imgAllocInfo = {};
		imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		CHECK_VULKAN(vmaCreateImage(*allocator, &imgInfo, &imgAllocInfo, &m_Image, &m_ImageAllocation, nullptr));

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
		device->EndCommandBuffer(cmd, true);

		vmaDestroyBuffer(*allocator, stagingBuffer.buffer, stagingBuffer.allocation);

		VkImageViewCreateInfo imageViewInfo = vkinit::imageview_create_info(internalFormat, m_Image, VK_IMAGE_ASPECT_COLOR_BIT);
		CHECK_VULKAN(vkCreateImageView(device->GetVulkanDevice(), &imageViewInfo, nullptr, &m_ImageView));

		VkSamplerCreateInfo samplerInfo = vkinit::image_sampler_create_info(VK_FILTER_NEAREST);
		vkCreateSampler(device->GetVulkanDevice(), &samplerInfo, nullptr, &m_Sampler);

		m_ImageDescriptorInfo = {};
		m_ImageDescriptorInfo.sampler = m_Sampler;
		m_ImageDescriptorInfo.imageView = m_ImageView;
		m_ImageDescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	VulkanTexture2D::VulkanTexture2D(const std::string& path)
		:m_Path(path)
	{
		PROFILE_FUNCTION();

		VulkanContext* context = VulkanContext::Get();

		std::shared_ptr<VulkanDevice> device = context->GetDevice();

		auto allocator = context->GetAllocator();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			PROFILE_SCOPE("stbi_load - VulkanTexture2D::VulkanTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 4);
		}
		PK_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		VkFormat internalFormat;
		VkDeviceSize imageSize = width * height * 4;

		internalFormat = VK_FORMAT_R8G8B8A8_UNORM;

		m_InternalFormat = internalFormat;
		//m_DataFormat = dataFormat;

		PK_CORE_ASSERT(internalFormat, "Format not supported!");

		VkBufferCreateInfo stagingBufferInfo = {};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferInfo.size = imageSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo stagingBufferAllocInfo = {};
		stagingBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		AllocatedBuffer stagingBuffer;

		CHECK_VULKAN(vmaCreateBuffer(*allocator, &stagingBufferInfo, &stagingBufferAllocInfo, &stagingBuffer.buffer, &stagingBuffer.allocation, nullptr));

		void* region;

		vmaMapMemory(*allocator, stagingBuffer.allocation, &region);
		memcpy(region, data, static_cast<size_t>(imageSize));
		vmaUnmapMemory(*allocator, stagingBuffer.allocation);

		stbi_image_free(data);

		VkExtent3D imageExtent;
		imageExtent.width = static_cast<uint32_t>(m_Width);
		imageExtent.height = static_cast<uint32_t>(m_Height);
		imageExtent.depth = 1;

		VkImageCreateInfo imgInfo = vkinit::image_create_info(internalFormat, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, imageExtent);

		VmaAllocationCreateInfo imgAllocInfo = {};
		imgAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		CHECK_VULKAN(vmaCreateImage(*allocator, &imgInfo, &imgAllocInfo, &m_Image, &m_ImageAllocation, nullptr));

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
		device->EndCommandBuffer(cmd, true);

		vmaDestroyBuffer(*allocator, stagingBuffer.buffer, stagingBuffer.allocation);

		VkImageViewCreateInfo imageViewInfo = vkinit::imageview_create_info(internalFormat, m_Image, VK_IMAGE_ASPECT_COLOR_BIT);
		CHECK_VULKAN(vkCreateImageView(device->GetVulkanDevice(), &imageViewInfo, nullptr, &m_ImageView));

		VkSamplerCreateInfo samplerInfo = vkinit::image_sampler_create_info(VK_FILTER_NEAREST);
		vkCreateSampler(device->GetVulkanDevice(), &samplerInfo, nullptr, &m_Sampler);

		m_ImageDescriptorInfo = {};
		m_ImageDescriptorInfo.sampler = m_Sampler;
		m_ImageDescriptorInfo.imageView = m_ImageView;
		m_ImageDescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		PROFILE_FUNCTION();

		//glDeleteTextures(1, &m_RendererID);
	}

	void VulkanTexture2D::SetData(void* data, uint32_t size)
	{
		PROFILE_FUNCTION();

		//uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		//PK_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		//glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void VulkanTexture2D::Bind(uint32_t slot) const
	{
		PROFILE_FUNCTION();

		//glBindTextureUnit(slot, m_RendererID);
	}
}
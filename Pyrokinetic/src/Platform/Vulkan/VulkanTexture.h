#pragma once

#include "Pyrokinetic/Rendering/Texture.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "backends/imgui_impl_vulkan.h"


namespace pk
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(uint32_t width, uint32_t height);
		VulkanTexture2D(const std::string& path);

		virtual ~VulkanTexture2D() override;
		
		virtual uint32_t GetWidth() const override {return m_Width;}
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		VkDescriptorImageInfo GetDescriptorInfo() { return m_ImageDescriptorInfo; }

		virtual uint32_t GetRendererID() override { return m_RendererID; }

		virtual void* GetImGuiTexture() override 
		{ 
			return m_ImTextureID; 
		}

		virtual std::string GetPath() override { return m_Path; }

		virtual bool operator == (const Texture& other) const override 
		{
			return  m_Path == ((VulkanTexture2D&)other).m_Path;
		}

	private:
		std::string m_Path;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_RendererID;
		VkFormat m_InternalFormat;
		VkImage m_Image;
		VmaAllocation m_ImageAllocation;
		VkImageView m_ImageView;
		VkSampler m_Sampler;
		VkDescriptorImageInfo m_ImageDescriptorInfo;

		VkDescriptorPool m_Pool;

		void* m_ImTextureID = nullptr;

	};
}
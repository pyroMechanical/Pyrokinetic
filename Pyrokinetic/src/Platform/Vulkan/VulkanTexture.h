#pragma once

#include "Pyrokinetic/Rendering/Texture.h"

#include <vulkan/vulkan.h>


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

		virtual uint32_t GetRendererID() { return m_RendererID; }

		virtual bool operator == (const Texture& other) const override 
		{ 
			return m_RendererID == ((VulkanTexture2D&)other).m_RendererID; 
		}

	private:
		std::string m_Path;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_RendererID;
		VkFormat m_InternalFormat, m_DataFormat;
	};
}
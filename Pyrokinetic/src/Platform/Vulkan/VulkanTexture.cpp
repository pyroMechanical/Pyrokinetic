#include "pkpch.h"
#include "VulkanTexture.h"

#include <vulkan/vulkan.h>

#include "stb_image.h"

namespace pk
{
	VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		PROFILE_FUNCTION();

		VkFormat internalFormat = VK_FORMAT_R8G8B8_SRGB;

		m_InternalFormat = internalFormat;
		//m_DataFormat = dataFormat;

		//glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		//glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		//glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	VulkanTexture2D::VulkanTexture2D(const std::string& path)
		:m_Path(path)
	{
		PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			PROFILE_SCOPE("stbi_load - VulkanTexture2D::VulkanTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		PK_CORE_ASSERT(data, "Failed to load image!");
		m_Width = width;
		m_Height = height;

		VkFormat internalFormat, dataFormat;

		if (channels == 4)
		{
			internalFormat = VK_FORMAT_R8G8B8A8_SRGB;
			//dataFormat = GL_RGBA;
		}
		if (channels == 3)
		{
			internalFormat = VK_FORMAT_R8G8B8_SRGB;
			//dataFormat = GL_RGB;
		}

		m_InternalFormat = internalFormat;
		//m_DataFormat = dataFormat;

		PK_CORE_ASSERT(internalFormat, "Format not supported!");

		//glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		//glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		//glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
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
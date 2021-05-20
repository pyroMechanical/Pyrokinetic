#include "pkpch.h"
#include "Texture.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/Vulkan/VulkanTexture.h"

namespace pk
{
	std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:     return std::make_shared<OpenGLTexture2D>(path);
		case RendererAPI::API::Vulkan:     return std::make_shared<VulkanTexture2D>(path);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:     return std::make_shared<OpenGLTexture2D>(width, height);
		case RendererAPI::API::Vulkan:     return std::make_shared<VulkanTexture2D>(width, height);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	SubTexture2D::SubTexture2D(const std::shared_ptr<Texture2D>& texture, const std::pair<uint8_t, uint8_t> coords, const std::pair<uint16_t, uint16_t> tileSize, const std::pair<uint8_t, uint8_t> spriteSize)
		: m_Texture(texture), m_SpriteCoords(coords), m_TileSize(tileSize), m_SpriteSize(spriteSize)
	{}

	SubTexture2D::SubTexture2D(const std::shared_ptr<Texture2D>& texture)
		: m_Texture(texture), m_SpriteCoords(0, 0), m_TileSize(texture->GetWidth(), texture->GetHeight()), m_SpriteSize(1, 1)
	{}

	SubTexture2D::SubTexture2D(const std::string& path, const std::pair<uint8_t, uint8_t> coords, const std::pair<uint16_t, uint16_t> tileSize, const std::pair<uint8_t, uint8_t> spriteSize)
		: m_Texture(Texture2D::Create(path)), m_SpriteCoords(coords), m_TileSize(tileSize), m_SpriteSize(spriteSize)
	{}

	SubTexture2D::SubTexture2D(const std::string& path)
		: m_Texture(Texture2D::Create(path)), m_SpriteCoords(0, 0), m_SpriteSize(1, 1)
	{
		m_TileSize = { (uint16_t) m_Texture->GetWidth(), (uint16_t) m_Texture->GetHeight() };
	}

	const std::array<glm::vec2, 4> SubTexture2D::GetTexCoords() const
	{
		std::array<glm::vec2, 4> texCoords;
		glm::vec2 min = { ((double)(m_SpriteCoords.first * m_TileSize.first)) / (double)m_Texture->GetWidth(), ((double)(m_SpriteCoords.second * m_TileSize.second)) / (double)m_Texture->GetHeight() };
		glm::vec2 max = { ((double)((m_SpriteCoords.first + m_SpriteSize.first) * m_TileSize.first)) / (double)m_Texture->GetWidth(), ((double)((m_SpriteCoords.second + m_SpriteSize.second) * m_TileSize.second)) / (double)m_Texture->GetHeight()};

		//glm::vec2 min = { ((double)(m_SpriteCoords.first * m_TileSize.first) + 0.5) / (double)m_Texture->GetWidth(), ((double)(m_SpriteCoords.second * m_TileSize.second) + 0.5) / (double)m_Texture->GetHeight() };
		//glm::vec2 max = { ((double)((m_SpriteCoords.first + m_SpriteSize.first) * m_TileSize.first) - 0.5) / (double)m_Texture->GetWidth(), ((double)((m_SpriteCoords.second + m_SpriteSize.second) * m_TileSize.second) - 0.5) / (double)m_Texture->GetHeight() };

		texCoords[0] = { min.x, min.y };
		texCoords[1] = { max.x, min.y };
		texCoords[2] = { max.x, max.y };
		texCoords[3] = { min.x, max.y };
		
		return texCoords;
	}

	const glm::vec2 SubTexture2D::GetMin() const
	{
		return { (double)(m_SpriteCoords.first * m_TileSize.first) / (double)m_Texture->GetWidth(), (double)(m_SpriteCoords.second * m_TileSize.second) / (double)m_Texture->GetHeight() };
	}

	const glm::vec2 SubTexture2D::GetMax() const
	{
		return { (double)((m_SpriteCoords.first + m_SpriteSize.first) * m_TileSize.first) / (double)m_Texture->GetWidth(), (double)((m_SpriteCoords.second + m_SpriteSize.second) * m_TileSize.second) / (double)m_Texture->GetHeight() };
	}

	const glm::vec2 SubTexture2D::GetTextureRatio() const
	{
		double widest;
		if(m_SpriteSize.first * m_TileSize.first >= m_SpriteSize.second * m_TileSize.second)
		{
			widest = (double)(m_SpriteSize.first * m_TileSize.first);
		}
		else
		{
			widest = (double)(m_SpriteSize.second * m_TileSize.second);
		}

		return { (double)(m_SpriteSize.first * m_TileSize.first) / widest, (double)(m_SpriteSize.second * m_TileSize.second) / widest };
	}

	std::shared_ptr<SubTexture2D> SubTexture2D::CreateFromCoordinates(const std::shared_ptr<Texture2D>& texture, const std::pair<uint8_t, uint8_t> coords, const std::pair<uint16_t, uint16_t> tileSize, const std::pair<uint8_t, uint8_t> spriteSize)
	{
		return std::make_shared<SubTexture2D>(texture, coords, tileSize, spriteSize);
	}

	std::shared_ptr<SubTexture2D> SubTexture2D::CreateFromPath(const std::string& path)
	{
		return std::make_shared<SubTexture2D>(path);
	}
}
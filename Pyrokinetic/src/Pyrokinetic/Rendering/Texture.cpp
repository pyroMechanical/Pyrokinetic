#include "pkpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/Vulkan/VulkanTexture.h"

namespace pk
{
	std::shared_ptr<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
		case API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case API::OpenGL:     return std::make_shared<OpenGLTexture2D>(path);
		case API::Vulkan:     return std::make_shared<VulkanTexture2D>(path);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer::GetAPI())
		{
		case API::None:       PK_CORE_ASSERT(false, "RenderAPI::None is currently not supported!"); return nullptr;
		case API::OpenGL:     return std::make_shared<OpenGLTexture2D>(width, height);
		case API::Vulkan:     return std::make_shared<VulkanTexture2D>(width, height);
		}

		PK_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	SubTexture2D::SubTexture2D(const std::shared_ptr<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max)
		: m_Texture(texture)
	{
		m_TexCoords[0] = { min.x, min.y };
		m_TexCoords[1] = { max.x, min.y };
		m_TexCoords[2] = { max.x, max.y };
		m_TexCoords[3] = { min.x, max.y };
	}

	std::shared_ptr<SubTexture2D> SubTexture2D::CreateFromCoordinates(const std::shared_ptr<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& tileSize, const glm::vec2& spriteSize)
	{
		glm::vec2 min = { coords.x * tileSize.x / texture->GetWidth(), coords.y * tileSize.y / texture->GetHeight() };
		glm::vec2 max = { (coords.x + spriteSize.x) * tileSize.x / texture->GetWidth(), (coords.y + spriteSize.y) * tileSize.y / texture->GetHeight() };
		PK_CORE_INFO("{0}", (coords.y + spriteSize.y) * tileSize.y);

		return std::make_shared<SubTexture2D>(texture, min, max);
	}
}
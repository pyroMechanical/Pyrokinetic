#pragma once

#include "Pyrokinetic/Core/Core.h"
#include <glm/glm.hpp>
#include <string>

namespace pk
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual uint32_t GetRendererID() = 0;

		virtual bool operator == (const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static std::shared_ptr<Texture2D> Create(uint32_t width, uint32_t height);
		static std::shared_ptr<Texture2D> Create(const std::string& path);
		
		
	};

	class SubTexture2D
	{
	public:
		SubTexture2D(const std::shared_ptr<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);


		const std::shared_ptr<Texture2D>& GetTexture() const { return m_Texture; }
		const glm::vec2* GetTexCoords() const { return m_TexCoords; }

		static std::shared_ptr<SubTexture2D> CreateFromCoordinates(const std::shared_ptr<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& tileSize, const glm::vec2& spriteSize = { 1, 1 });
	private:
		std::shared_ptr<Texture2D> m_Texture;
		glm::vec2 m_TexCoords[4];
	};


}
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

		virtual void* GetImGuiTexture() = 0;

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
		SubTexture2D(const std::shared_ptr<Texture2D>& texture, const std::pair<uint8_t, uint8_t> coords, const std::pair<uint16_t, uint16_t> tileSize, const std::pair<uint8_t, uint8_t> spriteSize);
		SubTexture2D(const std::shared_ptr<Texture2D>& texture);
		SubTexture2D(const std::string& path, const std::pair<uint8_t, uint8_t> coords, const std::pair<uint16_t, uint16_t> tileSize, const std::pair<uint8_t, uint8_t> spriteSize);
		SubTexture2D(const std::string& path);

		const std::shared_ptr<Texture2D>& GetTexture() const { return m_Texture; }
		const std::array<glm::vec2, 4> GetTexCoords() const;
		const glm::vec2 GetMin() const;
		const glm::vec2 GetMax() const;
		const glm::vec2 GetTextureRatio() const;

		static std::shared_ptr<SubTexture2D> CreateFromCoordinates(const std::shared_ptr<Texture2D>& texture, const std::pair<uint8_t, uint8_t> coords = { 0, 0 }, const std::pair<uint16_t, uint16_t> tileSize = { 0, 0 }, const std::pair<uint8_t, uint8_t> spriteSize = { 1, 1 });
		static std::shared_ptr<SubTexture2D> CreateFromPath(const std::string& path);
	private:
		std::shared_ptr<Texture2D> m_Texture;
		std::pair<uint16_t, uint16_t> m_TileSize;
		std::pair<uint8_t, uint8_t> m_SpriteCoords;
		std::pair<uint8_t, uint8_t> m_SpriteSize;
	};


}
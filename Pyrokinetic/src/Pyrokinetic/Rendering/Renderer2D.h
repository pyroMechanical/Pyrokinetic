#pragma once

#include "Camera.h"
#include "Texture.h"

namespace pk
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation);
		static void DrawQuad(const glm::vec3& position,const glm::vec2& size, const glm::vec4& color, float rotation);
							 
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, float rotation, float tileFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, float rotation, float tileFactor = 1.0f);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<SubTexture2D>& texture, float rotation, float tileFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<SubTexture2D>& texture, float rotation, float tileFactor = 1.0f);


		struct Statistics
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;

			uint32_t GetTotalVertexCount() { return quadCount * 4; }
			uint32_t GetTotalIndexCount() { return quadCount * 6; }
		};

		static void ResetStats();
		static Statistics& GetStats();
	private:
		static void StartNewBatch();
	};
}
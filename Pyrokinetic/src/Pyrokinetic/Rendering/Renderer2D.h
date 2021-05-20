#pragma once

#include "Camera.h"
#include "Texture.h"

namespace pk
{
#define PK_COLOR_DEFAULT glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}
#define PK_COLOR_NOSHADER glm::vec4{1.0f, 0.0f, 1.0f, 1.0f}
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();


		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, const std::shared_ptr<Texture2D>& texture, float rotation, float tileFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, const std::shared_ptr<Texture2D>& texture, float rotation, float tileFactor = 1.0f);

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<SubTexture2D>& texture, float rotation, float tileFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<SubTexture2D>& texture, float rotation, float tileFactor = 1.0f);

		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, const std::shared_ptr<Texture2D>& texture, float tileFactor = 1.0f);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, const std::shared_ptr<SubTexture2D>& texture, float tileFactor = 1.0f);

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
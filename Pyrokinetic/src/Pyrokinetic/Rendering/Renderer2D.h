#pragma once

#include "Camera.h"
#include "Texture.h"

namespace Pyrokinetic
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, float rotation);
		static void DrawQuad(const glm::vec3& position,const glm::vec2& size, const glm::vec4& color, float rotation);
							 
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float rotation, float tileFactor = 1.0f);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float rotation, float tileFactor = 1.0f);
	};
}
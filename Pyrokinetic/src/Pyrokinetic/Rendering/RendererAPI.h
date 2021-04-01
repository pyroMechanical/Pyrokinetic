#pragma once
#include "GraphicsContext.h"
#include <glm/glm.hpp>
#include <memory>

namespace pk
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, Vulkan = 2, DirectX11 = 3
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;
		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void SetContext(GraphicsContext* context) = 0;

		//virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		inline static API GetAPI() { return s_API; }
		inline static void SetAPI(API api) { s_API = api; }

		static std::unique_ptr<RendererAPI> Create();
	private:
		inline static API s_API;
	};
}
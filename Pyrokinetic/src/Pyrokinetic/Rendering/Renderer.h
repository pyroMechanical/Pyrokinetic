#pragma once

#include "RenderCommand.h"

#include "Camera.h"

#include "Pipeline.h"

#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

#include "RenderCommandBuffer.h"

#include <unordered_map>

namespace pk
{

	class Renderer
	{
	public:
		static void Init(const std::unique_ptr<GraphicsContext>& context);
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera); //TODO take in scene params
		static void EndScene();

		static void BeginRenderPass(std::shared_ptr<RenderPass>& renderPass);

		static void EndRenderPass();

		//static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static void Submit(const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer, const uint32_t indexCount);

		static void Flush();

		static GraphicsContext* GetContext() { return s_Context; }

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
		
		static void AddRenderPass(const std::string& name, std::shared_ptr<RenderPass> renderPass) { s_RenderPasses.insert(std::pair<std::string, std::shared_ptr<RenderPass>>(name, renderPass)); }
		static std::shared_ptr<RenderPass> GetRenderPass(const std::string& name) { return s_RenderPasses.at(name); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_SceneData;

		inline static GraphicsContext* s_Context;

		inline static std::shared_ptr<RenderCommandBuffer> s_RenderCommandBuffer;

		inline static std::unordered_map <std::string, std::shared_ptr<RenderPass>> s_RenderPasses;
	};
}
#include "pkpch.h"
#include "Renderer.h"
#include "Renderer2D.h"

namespace pk
{
	std::unique_ptr<Renderer::SceneData> Renderer::s_SceneData = std::make_unique<Renderer::SceneData>();

	void Renderer::Init(const std::unique_ptr<GraphicsContext>& context)
	{
		PROFILE_FUNCTION();

		s_Context = context.get();

		s_RenderCommandBuffer = RenderCommandBuffer::Create();


		RenderCommand::Init(s_Context);
		Renderer2D::Init();
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::BeginRenderPass(std::shared_ptr<RenderPass>& renderPass)
	{
		s_RenderCommandBuffer->BeginRenderPass(renderPass);
	}

	void Renderer::EndRenderPass()
	{
		s_RenderCommandBuffer->EndRenderPass();
	}

	void Renderer::Submit(const std::shared_ptr<Pipeline>& pipeline, const std::shared_ptr<VertexBuffer>& vertexBuffer, const std::shared_ptr<IndexBuffer>& indexBuffer, const uint32_t indexCount)
	{
		s_RenderCommandBuffer->Submit(pipeline, vertexBuffer, indexBuffer, indexCount);
	}

	void Renderer::Flush()
	{
		s_RenderCommandBuffer->Flush();
	}
}
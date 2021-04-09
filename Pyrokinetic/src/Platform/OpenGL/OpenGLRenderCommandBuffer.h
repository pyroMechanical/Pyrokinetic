#pragma once
#include "Pyrokinetic/Rendering/RenderCommandBuffer.h"

#include <vector>
#include <functional>

#include "OpenGLRenderPass.h"
#include "OpenGLFramebuffer.h"

namespace pk
{
	class OpenGLRenderCommandBuffer : public RenderCommandBuffer
	{
	public:
		OpenGLRenderCommandBuffer();
		~OpenGLRenderCommandBuffer();

		virtual void BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass) override;
		virtual void EndRenderPass() override;

		virtual void Submit(const std::shared_ptr<Pipeline>& pipeline,
			const std::shared_ptr<VertexBuffer>& vertexBuffer,
			const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t indexCount) override;

		virtual void Flush() override;

	private:
		std::vector<std::function<void(OpenGLFramebuffer&)>> m_Queue;

		std::vector<OpenGLRenderPass*> m_RenderPasses;
	};
}
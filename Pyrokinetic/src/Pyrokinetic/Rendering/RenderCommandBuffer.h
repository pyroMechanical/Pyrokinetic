#pragma once

#include "RenderPass.h"
#include "Pipeline.h"
#include <memory>

namespace pk
{
	class RenderCommandBuffer
	{
	public:
		virtual void BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass) = 0;
		virtual void EndRenderPass() = 0;

		virtual void Submit(const std::shared_ptr<Pipeline>& pipeline,
			const std::shared_ptr<VertexBuffer>& vertexBuffer,
			const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t indexCount) = 0;

		virtual void Flush() = 0;

		static std::shared_ptr<RenderCommandBuffer> Create();
	};
}
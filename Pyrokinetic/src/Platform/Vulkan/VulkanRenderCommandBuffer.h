#pragma once

#include "Pyrokinetic/Rendering/RenderCommandBuffer.h"

#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <functional>

namespace pk
{
	class VulkanRenderCommandBuffer : public RenderCommandBuffer
	{
	public:
		VulkanRenderCommandBuffer();
		~VulkanRenderCommandBuffer();

		virtual void BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass) override;
		virtual void EndRenderPass() override;

		virtual void Submit(const std::shared_ptr<Pipeline>& pipeline,
			const std::shared_ptr<VertexBuffer>& vertexBuffer,
			const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t indexCount) override;

		virtual void Flush() override;
	private:
		std::vector<std::function<void(const VkCommandBuffer& drawCommandBuffer, const VulkanFramebuffer& framebuffer)>> m_Queue;
		std::vector<VulkanRenderPass*> m_RenderPasses;
	};
}
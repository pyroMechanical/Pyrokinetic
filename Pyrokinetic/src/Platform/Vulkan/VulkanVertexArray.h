#pragma once

#include "Pyrokinetic/Rendering/VertexArray.h"

namespace pk
{
	class VulkanVertexArray : public VertexArray
	{
	public:
		VulkanVertexArray();

		virtual ~VulkanVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;


		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
	private:
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;

		uint32_t m_RendererID;
	};
}
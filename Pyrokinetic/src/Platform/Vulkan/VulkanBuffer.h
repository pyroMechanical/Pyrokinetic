#pragma once
#include "Pyrokinetic/Rendering/Buffer.h"
#include "VulkanMemory.h"
#include "VkTypes.h"
#include <vector>

namespace pk
{
	class VulkanVertexBuffer : public VertexBuffer
	{
		struct VertexInputDescription
		{
			std::vector<VkVertexInputBindingDescription> m_Bindings;
			std::vector<VkVertexInputAttributeDescription> m_Attributes;
		};

	public:
		VulkanVertexBuffer(uint32_t size);
		VulkanVertexBuffer(float* vertices, uint32_t size);
		virtual ~VulkanVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }


		VertexInputDescription GetVertexInputDescription();

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
		AllocatedBuffer m_Buffer;

		VmaAllocator* allocator;

	};

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* vertices, uint32_t count);
		virtual ~VulkanIndexBuffer();


		virtual uint32_t GetCount() const { return m_Count; }

		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
		AllocatedBuffer m_Buffer;

		VmaAllocator* allocator;
	};
}
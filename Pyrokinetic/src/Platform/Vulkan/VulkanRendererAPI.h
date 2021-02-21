#pragma once
#include "Pyrokinetic/Rendering/RendererAPI.h"
#include "Pyrokinetic/Rendering/GraphicsContext.h"
#include "Platform/Vulkan/VulkanContext.h" //figure out how to actually include this
#include "Pyrokinetic/Core/Window.h"


namespace Pyrokinetic
{

	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

		void SetContext(VulkanContext* context) { m_Context = context; }
	private:
		VulkanContext* m_Context;
	};

}
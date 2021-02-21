#include "pkpch.h"
#include "VulkanRendererAPI.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Pyrokinetic
{


	void VulkanRendererAPI::Init()
	{
		PROFILE_FUNCTION();

	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		PROFILE_FUNCTION();

		m_Context->m_WindowExtent = { width - x, height - y };
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
		PROFILE_FUNCTION();

		VkClearValue clearValue;
		clearValue.color = { color.r, color.g, color.b, color.a };
		m_Context->m_ClearValue = clearValue;
	}

	void VulkanRendererAPI::Clear()
	{
		PROFILE_FUNCTION();

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
	{
		PROFILE_FUNCTION();

		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		//glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}
}
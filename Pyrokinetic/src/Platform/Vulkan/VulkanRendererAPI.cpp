#include "pkpch.h"
#include "VulkanRendererAPI.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace pk
{
	void VulkanRendererAPI::Init()
	{
		PROFILE_FUNCTION();
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		PROFILE_FUNCTION();

		m_Context->SetViewport({ width - x, height - y });
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
		PROFILE_FUNCTION();

		VkClearValue clearValue;
		clearValue.color = { color.r, color.g, color.b, color.a };
		//m_Context->clearValue = clearValue;
	}

	void VulkanRendererAPI::Clear()
	{
		PROFILE_FUNCTION();
	}

	void VulkanRendererAPI::Begin()
	{
	}

	void VulkanRendererAPI::End()
	{
	}
}
#pragma once
#include "Pyrokinetic/Rendering/GraphicsContext.h"
#include <vulkan/vulkan.h>
struct GLFWwindow;

namespace Pyrokinetic
{
	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void SetVSync(bool enabled) override;

	private:
		void InitVulkan();
		void CreateSwapchain();
		void InitCommands();
		void InitDefaultRenderpass();
		void InitFramebuffers();
		void InitSyncStructures();
		void InitPipelines();
		void Cleanup();
		/// <summary>
		/// TODO: move/rewrite these functions for other parts of the API;
		/// </summary>
		bool load_shader_module(const char* filePath, VkShaderModule* outShaderModule);

		GLFWwindow* m_WindowHandle;
		bool VSync = false;
		VkExtent2D m_WindowExtent;
		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkSurfaceKHR m_Surface;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;
		VkSwapchainKHR m_Swapchain;
		VkFormat m_SwapchainImageFormat;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
		VkQueue m_GraphicsQueue;
		uint32_t m_GraphicsQueueFamily;
		VkCommandPool m_CommandPool;
		VkCommandBuffer m_MainCommandBuffer;
		VkRenderPass m_RenderPass;
		std::vector<VkFramebuffer> m_Framebuffers;
		VkSemaphore m_PresentSemaphore, m_RenderSemaphore;
		VkFence m_RenderFence;
		VkClearValue m_ClearValue;
		VkPipelineLayout m_TrianglePipelineLayout;
		VkPipeline m_TrianglePipeline;

		friend class VulkanRendererAPI;
	};
}
#pragma once
#include "Pyrokinetic/Rendering/GraphicsContext.h"
#include "VulkanMemory.h"
#include "VulkanShader.h"
#include "VkTypes.h"
#include <vulkan/vulkan.h>
#include <deque>
struct GLFWwindow;

#define CHECK_VULKAN(x) \
		{ \
			VkResult err = x;								\
			if (err)										\
			{												\
				PK_CORE_ERROR("Vulkan Error: {0}", err);	\
			}												\
		}													\

namespace pk
{
	constexpr unsigned int FRAME_OVERLAP = 2;

	class VulkanContext : public GraphicsContext
	{
		struct DeletionQueue
		{
			std::deque<std::function<void()>> deletors;

			void push_function(std::function<void()>&& function)
			{
				deletors.push_front(function);
			}

			void flush()
			{
				for (auto it = deletors.rbegin(); it != deletors.rend(); ++it)
				{
					(*it)();
				}
				deletors.clear();
			}
		};

		

	public:
		VulkanContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void SetVSync(bool enabled) override;
		void SetViewport(VkExtent2D extent);

		FrameData& GetCurrentFrame(unsigned int currentFrame) { return frameData[currentFrame % FRAME_OVERLAP]; };

		void RebuildSwapchain();

		DeletionQueue mainDeletionQueue;
		DeletionQueue swapchainDeletionQueue;
		bool VSync = false;
		VkExtent2D windowExtent;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		VkSwapchainKHR swapchain;
		uint32_t swapchainImageIndex = 0;
		VkFormat swapchainImageFormat;
		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
		VkQueue graphicsQueue;
		uint32_t graphicsQueueFamily;
		FrameData frameData[FRAME_OVERLAP];
		uint32_t frameNumber;
		VkRenderPass renderPass;
		std::vector<VkFramebuffer> framebuffers;
		VkClearValue clearValue;
		VkPipelineLayout trianglePipelineLayout;
		VkPipeline trianglePipeline;
		std::unordered_map<std::string, VulkanShader> shaders;
		std::unordered_map<std::string, VkPipeline> pipelines;

		VmaAllocator* allocator;

	private:
		void InitVulkan();
		void CreateSwapchain();
		void InitCommands();
		void InitDefaultRenderPass();
		void InitFramebuffers();
		void InitSyncStructures();
		void InitDescriptors();
		void InitPipelines();
		void Cleanup();
		/// <summary>
		/// TODO: move/rewrite these functions for other parts of the API;
		/// </summary>
		bool load_shader_module(const char* filePath, VkShaderModule* outShaderModule);
		AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

		GLFWwindow* m_WindowHandle;
		
		
	};
}
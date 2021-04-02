#pragma once
#include "Pyrokinetic/Rendering/GraphicsContext.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
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
	struct InitInfo
	{
		VkInstance                      Instance;
		VkPhysicalDevice                PhysicalDevice;
		VkDevice                        Device;
		uint32_t                        QueueFamily;
		VkQueue                         Queue;
		VkPipelineCache                 PipelineCache;
		VkDescriptorPool                DescriptorPool;
		uint32_t                        Subpass;
		uint32_t                        MinImageCount;          // >= 2
		uint32_t                        ImageCount;             // >= MinImageCount
		VkSampleCountFlagBits           MSAASamples;            // >= VK_SAMPLE_COUNT_1_BIT
		const VkAllocationCallbacks* Allocator;
		void                            (*CheckVkResultFn)(VkResult err);
	};

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

		uint32_t GetWidth() { return windowExtent.width; }
		uint32_t GetHeight() { return windowExtent.height; }

		FrameData& GetCurrentFrame(unsigned int currentFrame) { return frameData[currentFrame % FRAME_OVERLAP]; };

		VmaAllocator* GetAllocator() { return &allocator; }

		void RebuildSwapchain();

		InitInfo CreateImGuiImplInfo();

		VkDevice& GetDevice() { return device; }

		VkRenderPass& GetRenderPass() { return renderPass; }

		std::vector<VkCommandBuffer> GetCommandBuffers()
		{
			std::vector<VkCommandBuffer> cmdBuffers;
			for(int i = 0; i < FRAME_OVERLAP; ++i)
			{
				cmdBuffers.push_back(frameData[i].mainCommandBuffer);
			}

			return cmdBuffers;
		}

		VkCommandBuffer& GetCurrentCommandBuffer()
		{
			return GetCurrentFrame(frameNumber).mainCommandBuffer;
		}

		VkFramebuffer& GetCurrentFramebuffer()
		{
			return framebuffers[frameNumber % FRAME_OVERLAP];
		}

		 VkCommandBuffer BeginImmediateExecute();

		 void EndImmediateExecute(VkCommandBuffer buffer);

		std::vector<VkFramebuffer> GetFramebuffers()
		{
			return framebuffers;
		}

		VkPipeline GetPipeline()
		{
			return trianglePipeline;
		}

		DeletionQueue mainDeletionQueue;
		DeletionQueue swapchainDeletionQueue;
		bool VSync = false;
		VkExtent2D windowExtent;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		VkSurfaceKHR surface;
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
		std::unordered_map<std::string, VulkanPipeline> pipelines;
		VkCommandPool immediateCommandPool;
		VkFence immediateFence;

		VkDescriptorSetLayout globalSetLayout;
		VkDescriptorPool descriptorPool;

		VmaAllocator allocator;

	private:
		void InitVulkan();
		void CreateSwapchain();
		void InitCommands();
		void InitImmediateCommands();
		void InitDefaultRenderPass();
		void InitFramebuffers();
		void InitSyncStructures();
		void InitDescriptors();
		void InitPipelines();
		void Cleanup();
		/// <summary>
		/// TODO: move/rewrite these functions for other parts of the API;
		/// </summary>
		bool load_shader_module(std::string& filePath, VkShaderModule* outShaderModule);
		AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

		GLFWwindow* m_WindowHandle;
		
		
	};
}
#pragma once
#include "Pyrokinetic/Rendering/GraphicsContext.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VkTypes.h"
#include "VkBootstrap.h"
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
		~VulkanContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void SetVSync(bool enabled) override {};
		void SetViewport(VkExtent2D extent);

		static VulkanContext* Get() { return s_Context; }

		VmaAllocator* GetAllocator() { return &m_Allocator; }

		InitInfo CreateImGuiImplInfo();

		std::shared_ptr<VulkanDevice> GetDevice() { return m_Device; }

		VulkanSwapchain GetSwapchain() { return m_Swapchain; }

		static vkb::Instance GetVkbInstance() { return m_Instance; }
		static VkInstance GetVulkanInstance() { return m_Instance.instance; }

		VkDescriptorSetLayout* GetDescriptorSetLayout() { return &m_GlobalDescriptorLayout; }

	private:

		GLFWwindow* m_WindowHandle;
		
		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::shared_ptr<VulkanDevice> m_Device;

		DeletionQueue m_MainDeletionQueue;

		inline static VulkanContext* s_Context;

		inline static vkb::Instance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPipelineCache m_PipelineCache = nullptr;
		
		VmaAllocator m_Allocator;
		VulkanSwapchain m_Swapchain{ m_WindowHandle };

		VkDescriptorSetLayout m_GlobalDescriptorLayout;
		VkDescriptorPool m_DescriptorPool;
	};
}
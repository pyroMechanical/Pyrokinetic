#include "pkpch.h"
#include "VulkanContext.h"
#include "VulkanPipelineBuilder.h"
#include "Pyrokinetic/Rendering/RenderCommand.h"
#include "VkBootstrap.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define PK_DEBUG

//TODO: remove from this file and move to appropriate API files
#include <fstream>

namespace pk 
{

	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		PK_CORE_ASSERT(m_WindowHandle, "Handle is null!");
		s_Context = this;
	}

	VulkanContext::~VulkanContext()
	{
		m_Swapchain.DestroyFramebuffers();
		//vmaDestroyAllocator(m_Allocator);
	}

	void VulkanContext::Init()
	{
		PROFILE_FUNCTION();

		vkb::InstanceBuilder instance_builder;
#ifdef PK_DEBUG
		instance_builder.request_validation_layers().set_debug_callback(
			[](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData)
			-> VkBool32
			{
				auto severity = vkb::to_string_message_severity(messageSeverity);
				auto type = vkb::to_string_message_type(messageType);
				if (severity == "VERBOSE") PK_CORE_TRACE("{0}, {1}", type, pCallbackData->pMessage);
				if (severity == "INFO") PK_CORE_INFO("{0}, {1}", type, pCallbackData->pMessage);
				if (severity == "WARNING") PK_CORE_WARN("{0}, {1}", type, pCallbackData->pMessage);
				if (severity == "ERROR") PK_CORE_ERROR("{0}, {1}", type, pCallbackData->pMessage);
				return VK_FALSE;
			}
		);
#endif
		instance_builder.require_api_version(1, 1, 0);

		auto system_info_return = vkb::SystemInfo::get_system_info();
		if (system_info_return)
		{
			auto system_info = system_info_return.value();
			if (system_info.is_layer_available("VK_LAYER_KHRONOS_validation"))
				instance_builder.enable_layer("VK_LAYER_KHRONOS_validation");
			if (system_info.validation_layers_available)
				instance_builder.enable_validation_layers();
		}


		auto instance_builder_result = instance_builder.build();

		vkb::Instance vkb_instance = instance_builder_result.value();

		m_Instance = vkb_instance;
		m_DebugMessenger = vkb_instance.debug_messenger;

		m_Swapchain.CreateSurface();

		m_PhysicalDevice = std::make_shared<VulkanPhysicalDevice>(m_Swapchain.GetVulkanSurface());

		m_Device = std::make_shared<VulkanDevice>(m_PhysicalDevice);

		m_Swapchain.CreateSwapchain(m_Device);

		RenderPassSpecification renderPassSpec = {};
		renderPassSpec.samples = 1;
		renderPassSpec.clearColor = { 0.5f, 0.0f, 0.0f, 1.0f };
		renderPassSpec.Attachments = { ImageFormat::RGBA8 };

		auto renderPass = RenderPass::Create(renderPassSpec);

		FramebufferSpecification framebufferSpec = {};
		framebufferSpec.renderPass = renderPass;
		framebufferSpec.width = 1280;
		framebufferSpec.height = 720;
		framebufferSpec.SwapchainTarget = true;

		m_Swapchain.CreateFramebuffers(framebufferSpec);

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_PhysicalDevice->GetVulkanPhysicalDevice();
		allocatorInfo.device = m_Device->GetVulkanDevice();
		allocatorInfo.instance = m_Instance.instance;
		vmaCreateAllocator(&allocatorInfo, &m_Allocator);

		VkDescriptorSetLayoutBinding cameraMatrixBinding = {};
		cameraMatrixBinding.binding = 0;
		cameraMatrixBinding.descriptorCount = 1;
		cameraMatrixBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		cameraMatrixBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo setInfo = {};
		setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		setInfo.pNext = nullptr;
		
		setInfo.bindingCount = 1;

		setInfo.flags = 0;
		setInfo.pBindings = &cameraMatrixBinding;

		vkCreateDescriptorSetLayout(m_Device->GetVulkanDevice(), &setInfo, nullptr, &m_GlobalDescriptorLayout);

		/*std::vector<VkDescriptorPoolSize> sizes =
		{
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10}
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = 0;
		pool_info.maxSets = 10;
		pool_info.poolSizeCount = (uint32_t)sizes.data();
		pool_info.pPoolSizes = sizes.data();

		vkCreateDescriptorPool(m_Device->GetVulkanDevice(), &pool_info, nullptr, &m_DescriptorPool);*/
	}

	void VulkanContext::SetViewport(VkExtent2D extent)
	{
		//m_Swapchain
	}

	void VulkanContext::SwapBuffers()
	{
		m_Swapchain.SwapBuffers();
	}

	//this is for Dear ImGui integration
	InitInfo VulkanContext::CreateImGuiImplInfo()
	{
		PROFILE_FUNCTION();

		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		VkDescriptorPool imguiPool;
		CHECK_VULKAN(vkCreateDescriptorPool(m_Device->GetVulkanDevice(), &pool_info, nullptr, &imguiPool));


		InitInfo info{};
		info.Instance = m_Instance.instance;
		info.PhysicalDevice = m_PhysicalDevice->GetVulkanPhysicalDevice();
		info.Device = m_Device->GetVulkanDevice();
		info.QueueFamily = m_Device->GetGraphicsQueueFamily();
		info.Queue = m_Device->GetGraphicsQueue();
		info.PipelineCache = VK_NULL_HANDLE;
		info.DescriptorPool = imguiPool;
		info.Subpass = 0;
		info.MinImageCount = FRAME_OVERLAP;
		info.ImageCount = FRAME_OVERLAP;
		info.CheckVkResultFn = [](VkResult result)
		{
			if (result)
			{
				PK_CORE_ERROR("Vulkan Error: {0}", result);
			}
		};

		return info;
	}
}
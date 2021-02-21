#include "pkpch.h"
#include "VulkanContext.h"
#include "VulkanPipelineBuilder.h"
#include "Pyrokinetic/Rendering/RenderCommand.h"
#include "VulkanInitializer.h"
#include "VkBootstrap.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//TODO: remove from this file and move to appropriate API files
#include <fstream>


#define CHECK_VULKAN(x) \
		{ \
			VkResult err = x;								\
			if (err)										\
			{												\
				PK_CORE_ERROR("Vulkan Error: {0}", err);	\
			}												\
		}													\

namespace Pyrokinetic 
{
	
	VulkanContext::VulkanContext(GLFWwindow* windowHandle) 
		: m_WindowHandle(windowHandle)
	{
		PK_CORE_ASSERT(m_WindowHandle, "Handle is null!");
		int width, height;
		glfwGetWindowSize(m_WindowHandle, &width, &height);
		m_WindowExtent = { (uint32_t)width, (uint32_t)height };
		m_ClearValue = {};
		m_ClearValue.color = { 0.01, 0.01, 0.01, 1.0 };
	}

	void VulkanContext::Init()
	{
		PROFILE_FUNCTION();
		reinterpret_cast<VulkanRendererAPI&>(RenderCommand::getRendererAPI()).SetContext(this);

		InitVulkan();
		CreateSwapchain();
		InitCommands();
		InitDefaultRenderpass();
		InitFramebuffers();
		InitSyncStructures();

		InitPipelines();
	}

	void VulkanContext::InitVulkan()
	{
		vkb::InstanceBuilder instance_builder;
#ifdef PK_DEBUG
		instance_builder.request_validation_layers(true);
#endif
		instance_builder.require_api_version(1, 1, 0).set_debug_callback(
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

		auto instance_builder_result = instance_builder.build();

		vkb::Instance vkb_instance = instance_builder_result.value();

		m_Instance = vkb_instance.instance;
		m_DebugMessenger = vkb_instance.debug_messenger;

		glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface);

		vkb::PhysicalDeviceSelector physical_device_selector{ vkb_instance };
		vkb::PhysicalDevice vkb_physical_device = physical_device_selector.set_minimum_version(1, 1).set_surface(m_Surface).select().value();

		vkb::DeviceBuilder device_builder{ vkb_physical_device };
		vkb::Device vkb_device = device_builder.build().value();

		m_PhysicalDevice = vkb_physical_device.physical_device;
		m_Device = vkb_device.device;

		/*PK_CORE_INFO("Vulkan Info:");
		PK_CORE_INFO("Vendor ID: {0}", m_PhysicalDevice.properties.vendorID);
		PK_CORE_INFO("Renderer: {0}", m_PhysicalDevice.properties.deviceName);
		PK_CORE_INFO("Vulkan Version: {0}", m_PhysicalDevice.properties.apiVersion);*/

		m_GraphicsQueue = vkb_device.get_queue(vkb::QueueType::graphics).value();
		m_GraphicsQueueFamily = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
	}

	void VulkanContext::CreateSwapchain()
	{

		vkb::SwapchainBuilder swapchain_builder{ m_PhysicalDevice, m_Device, m_Surface };

		if (VSync) swapchain_builder.set_desired_present_mode(VK_PRESENT_MODE_FIFO_RELAXED_KHR);

		vkb::Swapchain vkb_swapchain = swapchain_builder.use_default_format_selection()
			.set_desired_extent(m_WindowExtent.width, m_WindowExtent.height)
			.build()
			.value();

		m_Swapchain = vkb_swapchain.swapchain;
		m_SwapchainImages = vkb_swapchain.get_images().value();
		m_SwapchainImageViews = vkb_swapchain.get_image_views().value();
		m_SwapchainImageFormat = vkb_swapchain.image_format;
	}

	void VulkanContext::InitCommands()
	{
		VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(m_GraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		CHECK_VULKAN(vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_CommandPool));

		VkCommandBufferAllocateInfo commandAllocInfo = vkinit::command_buffer_allocate_info(m_CommandPool, 1);
		CHECK_VULKAN(vkAllocateCommandBuffers(m_Device, &commandAllocInfo, &m_MainCommandBuffer));
	}

	void VulkanContext::InitDefaultRenderpass()
	{
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = m_SwapchainImageFormat;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment_ref = {};
		color_attachment_ref.attachment = 0;
		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment_ref;

		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = 1;
		render_pass_info.pAttachments = &color_attachment;

		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass;

		CHECK_VULKAN(vkCreateRenderPass(m_Device, &render_pass_info, nullptr, &m_RenderPass));
	}

	void VulkanContext::InitFramebuffers()
	{
		VkFramebufferCreateInfo fb_info = {};
		fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fb_info.pNext = nullptr;

		fb_info.renderPass = m_RenderPass;
		fb_info.attachmentCount = 1;
		fb_info.width = m_WindowExtent.width;
		fb_info.height = m_WindowExtent.height;
		fb_info.layers = 1;

		const uint32_t swapchain_imagecount = m_SwapchainImages.size();
		m_Framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

		for (int i = 0; i < swapchain_imagecount; i++)
		{
			fb_info.pAttachments = &m_SwapchainImageViews[i];
			CHECK_VULKAN(vkCreateFramebuffer(m_Device, &fb_info, nullptr, &m_Framebuffers[i]));
		}
	}

	void VulkanContext::InitSyncStructures()
	{
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;

		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		CHECK_VULKAN(vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_RenderFence));

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		CHECK_VULKAN(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_PresentSemaphore));
		CHECK_VULKAN(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_RenderSemaphore));
	}

	void VulkanContext::Cleanup()
	{

		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);

		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

		for (int i = 0; i < m_Framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(m_Device, m_Framebuffers[i], nullptr);
			vkDestroyImageView(m_Device, m_SwapchainImageViews[i], nullptr);
		}

		for (int i = 0; i < m_SwapchainImageViews.size(); i++)
		{
			vkDestroyImageView(m_Device, m_SwapchainImageViews[i], nullptr);
		}

		vkDestroyDevice(m_Device, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
		vkDestroyInstance(m_Instance, nullptr);
	
	}

	void VulkanContext::SwapBuffers() //Draw function;
	{
		PROFILE_FUNCTION();
		
		CHECK_VULKAN(vkWaitForFences(m_Device, 1, &m_RenderFence, true, 1000000000));
		CHECK_VULKAN(vkResetFences(m_Device, 1, &m_RenderFence));

		uint32_t swapchainImageIndex;
		CHECK_VULKAN(vkAcquireNextImageKHR(m_Device, m_Swapchain, 1000000000, m_PresentSemaphore, nullptr, &swapchainImageIndex));

		CHECK_VULKAN(vkResetCommandBuffer(m_MainCommandBuffer, 0));
		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBeginInfo.pNext = nullptr;
		cmdBeginInfo.pInheritanceInfo = nullptr;
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		CHECK_VULKAN(vkBeginCommandBuffer(m_MainCommandBuffer, &cmdBeginInfo));

		VkRenderPassBeginInfo rpInfo = {};
		rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rpInfo.pNext = nullptr;

		rpInfo.renderPass = m_RenderPass;
		rpInfo.renderArea.offset.x = 0;
		rpInfo.renderArea.offset.y = 0;
		rpInfo.renderArea.extent = m_WindowExtent;
		rpInfo.framebuffer = m_Framebuffers[swapchainImageIndex];
		rpInfo.clearValueCount = 1;
		rpInfo.pClearValues = &m_ClearValue;

		vkCmdBeginRenderPass(m_MainCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_MainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_TrianglePipeline);
		vkCmdDraw(m_MainCommandBuffer, 3, 1, 0, 0);
		vkCmdEndRenderPass(m_MainCommandBuffer);

		CHECK_VULKAN(vkEndCommandBuffer(m_MainCommandBuffer));

		VkSubmitInfo submit = {};
		submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit.pNext = nullptr;

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		submit.pWaitDstStageMask = &waitStage;

		submit.waitSemaphoreCount = 1;
		submit.pWaitSemaphores = &m_PresentSemaphore;

		submit.signalSemaphoreCount = 1;
		submit.pSignalSemaphores = &m_RenderSemaphore;

		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &m_MainCommandBuffer;

		CHECK_VULKAN(vkQueueSubmit(m_GraphicsQueue, 1, &submit, m_RenderFence));

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;

		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.swapchainCount = 1;

		presentInfo.pWaitSemaphores = &m_RenderSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &swapchainImageIndex;

		CHECK_VULKAN(vkQueuePresentKHR(m_GraphicsQueue, &presentInfo));
	}

	void VulkanContext::SetVSync(bool enabled)
	{
		
	}

	//TODO: move these functions to other classes in the API. *extremely* temporary!

	bool VulkanContext::load_shader_module(const char* filePath /* TODO: replace with std::string& */, VkShaderModule* outShaderModule)
	{
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);

		if(!file.is_open()) return false;

		size_t fileSize = (size_t)file.tellg();

		std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

		file.seekg(0);

		file.read((char*)buffer.data(), fileSize);

		file.close();

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = nullptr;

		createInfo.codeSize = buffer.size() * sizeof(uint32_t);
		createInfo.pCode = buffer.data();

		VkShaderModule shaderModule;

		if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			return false;
		}

		*outShaderModule = shaderModule;
		return true;
	}

	void VulkanContext::InitPipelines()
	{
		VkShaderModule triangleFragShader;
		if (!load_shader_module("assets/shaders/triangle.frag.spv", &triangleFragShader))
		{
			PK_CORE_ERROR("Error when building triangle fragment shader!");
		}
		else
		{
			PK_CORE_INFO("Triangle fragment shader loaded successfully!");
		}

		VkShaderModule triangleVertShader;
		if (!load_shader_module("assets/shaders/triangle.vert.spv", &triangleVertShader))
		{
			PK_CORE_ERROR("Error when building triangle Vertex shader!");
		}
		else
		{
			PK_CORE_INFO("Triangle vertex shader loaded successfully!");
		}

		VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();

		CHECK_VULKAN(vkCreatePipelineLayout(m_Device, &pipeline_layout_info, nullptr, &m_TrianglePipelineLayout));

		VulkanPipelineBuilder pipelineBuilder;

		pipelineBuilder.m_ShaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, triangleVertShader));
		pipelineBuilder.m_ShaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

		pipelineBuilder.m_VertexInputInfo = vkinit::vertex_input_state_create_info();
		pipelineBuilder.m_InputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		
		pipelineBuilder.m_Viewport.x = 0.0f;
		pipelineBuilder.m_Viewport.y = 0.0f;
		pipelineBuilder.m_Viewport.width = (float)m_WindowExtent.width;
		pipelineBuilder.m_Viewport.height = (float)m_WindowExtent.height;
		pipelineBuilder.m_Viewport.minDepth = 0.0f;
		pipelineBuilder.m_Viewport.maxDepth = 1.0f;

		pipelineBuilder.m_Scissor.offset = { 0, 0 };
		pipelineBuilder.m_Scissor.extent = m_WindowExtent;

		pipelineBuilder.m_Rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

		pipelineBuilder.m_Multisampling = vkinit::multisampling_state_create_info();

		pipelineBuilder.m_ColorBlendAttachment = vkinit::color_blend_attachment_state();

		pipelineBuilder.m_PipelineLayout = m_TrianglePipelineLayout;

		m_TrianglePipeline = pipelineBuilder.BuildPipeline(m_Device, m_RenderPass);
	}
}
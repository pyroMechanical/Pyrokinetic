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

namespace pk 
{

	VulkanContext::VulkanContext(GLFWwindow* windowHandle) 
		: m_WindowHandle(windowHandle)
	{
		PK_CORE_ASSERT(m_WindowHandle, "Handle is null!");

		int width, height;
		glfwGetFramebufferSize(m_WindowHandle, &width, &height);
		windowExtent = { (uint32_t)width, (uint32_t)height };
		clearValue = {};
		clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
		frameNumber = 0;
	}

	void VulkanContext::Init()
	{
		PROFILE_FUNCTION();

		InitVulkan();
		CreateSwapchain();
		InitCommands();
		InitImmediateCommands();
		InitDefaultRenderPass();
		InitFramebuffers();
		InitSyncStructures();
		InitDescriptors();
		InitPipelines();
	}

	void VulkanContext::InitVulkan()
	{
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

		instance = vkb_instance.instance;
		debugMessenger = vkb_instance.debug_messenger;

		glfwCreateWindowSurface(instance, m_WindowHandle, nullptr, &surface);

		vkb::PhysicalDeviceSelector physical_device_selector{ vkb_instance };
		vkb::PhysicalDevice vkb_physical_device = physical_device_selector.set_minimum_version(1, 1).set_surface(surface).select().value();

		vkb::DeviceBuilder device_builder{ vkb_physical_device };
		vkb::Device vkb_device = device_builder.build().value();

		physicalDevice = vkb_physical_device.physical_device;
		device = vkb_device.device;



		/*PK_CORE_INFO("Vulkan Info:");
		PK_CORE_INFO("Vendor ID: {0}", physicalDevice.properties.vendorID);
		PK_CORE_INFO("Renderer: {0}", physicalDevice.properties.deviceName);
		PK_CORE_INFO("Vulkan Version: {0}", physicalDevice.properties.apiVersion);*/

		graphicsQueue = vkb_device.get_queue(vkb::QueueType::graphics).value();
		graphicsQueueFamily = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = physicalDevice;
		allocatorInfo.device = device;
		allocatorInfo.instance = instance;
		vmaCreateAllocator(&allocatorInfo, &allocator);
	}

	void VulkanContext::CreateSwapchain()
	{

		vkb::SwapchainBuilder swapchain_builder{ physicalDevice, device, surface };

		if (VSync) swapchain_builder.set_desired_present_mode(VK_PRESENT_MODE_FIFO_RELAXED_KHR);

		VkSurfaceFormatKHR surfaceFormat{};
		surfaceFormat.format = VK_FORMAT_R8G8B8A8_SRGB;
		surfaceFormat.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;

		vkb::Swapchain vkb_swapchain = swapchain_builder.set_desired_format(surfaceFormat)
			.set_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.set_desired_extent(windowExtent.width, windowExtent.height)
			.build()
			.value();

		swapchain = vkb_swapchain.swapchain;
		swapchainImages = vkb_swapchain.get_images().value();
		swapchainImageViews = vkb_swapchain.get_image_views().value();
		swapchainImageFormat = vkb_swapchain.image_format;

		swapchainDeletionQueue.push_function([=]() {vkDestroySwapchainKHR(device, swapchain, nullptr); });
	}

	void VulkanContext::InitCommands()
	{

		for (int i = 0; i < FRAME_OVERLAP; i++)
		{
			VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
			CHECK_VULKAN(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &frameData[i].commandPool));
			

			VkCommandBufferAllocateInfo commandAllocInfo = vkinit::command_buffer_allocate_info(frameData[i].commandPool, 1);
			CHECK_VULKAN(vkAllocateCommandBuffers(device, &commandAllocInfo, &frameData[i].mainCommandBuffer));

			swapchainDeletionQueue.push_function([=]() {vkDestroyCommandPool(device, frameData[i].commandPool, nullptr); });
		}
	}

	void VulkanContext::InitImmediateCommands()
	{
		VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		CHECK_VULKAN(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &immediateCommandPool));

		mainDeletionQueue.push_function([=]() {vkDestroyCommandPool(device, immediateCommandPool, nullptr); });
	}

	void VulkanContext::InitDefaultRenderPass()
	{
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = swapchainImageFormat;
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

		CHECK_VULKAN(vkCreateRenderPass(device, &render_pass_info, nullptr, &renderPass));

		swapchainDeletionQueue.push_function([=]() {vkDestroyRenderPass(device, renderPass, nullptr); });
	}

	void VulkanContext::InitFramebuffers()
	{
		VkFramebufferCreateInfo fb_info = {};
		fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fb_info.pNext = nullptr;

		fb_info.renderPass = renderPass;
		fb_info.attachmentCount = 1;
		fb_info.width = windowExtent.width;
		fb_info.height = windowExtent.height;
		fb_info.layers = 1;

		const uint32_t swapchain_imagecount = swapchainImages.size();
		framebuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

		for (int i = 0; i < swapchain_imagecount; i++)
		{
			fb_info.pAttachments = &swapchainImageViews[i];
			CHECK_VULKAN(vkCreateFramebuffer(device, &fb_info, nullptr, &framebuffers[i]));
			swapchainDeletionQueue.push_function([=]() {
				vkDestroyFramebuffer(device, framebuffers[i], nullptr); 
				vkDestroyImageView(device, swapchainImageViews[i], nullptr);
				});
		}
	}

	void VulkanContext::InitSyncStructures()
	{
		for (int i = 0; i < FRAME_OVERLAP; i++)
		{
			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.pNext = nullptr;

			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			CHECK_VULKAN(vkCreateFence(device, &fenceCreateInfo, nullptr, &GetCurrentFrame(i).renderFence));

			mainDeletionQueue.push_function([=]() {vkDestroyFence(device, GetCurrentFrame(i).renderFence, nullptr); });

			VkSemaphoreCreateInfo semaphoreCreateInfo = {};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphoreCreateInfo.pNext = nullptr;
			semaphoreCreateInfo.flags = 0;

			CHECK_VULKAN(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &GetCurrentFrame(i).presentSemaphore));
			CHECK_VULKAN(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &GetCurrentFrame(i).renderSemaphore));

			mainDeletionQueue.push_function([=]() {
				vkDestroySemaphore(device, GetCurrentFrame(i).presentSemaphore, nullptr);
				vkDestroySemaphore(device, GetCurrentFrame(i).renderSemaphore, nullptr);
				});
		}

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;

		CHECK_VULKAN(vkCreateFence(device, &fenceCreateInfo, nullptr, &immediateFence));

		mainDeletionQueue.push_function([=]() {vkDestroyFence(device, immediateFence, nullptr); });
	}

	void VulkanContext::InitDescriptors()
	{

		std::vector<VkDescriptorPoolSize> sizes =
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

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = 0;
		poolInfo.maxSets = 1000;
		poolInfo.poolSizeCount = (uint32_t)sizes.size();
		poolInfo.pPoolSizes = sizes.data();

		CHECK_VULKAN(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool));

		for (int i = 0; i < FRAME_OVERLAP; i++)
		{
			//frameData[i].
		}

		mainDeletionQueue.push_function([=]() {
			vkDestroyDescriptorPool(device, descriptorPool, nullptr);
			});
	}

	void VulkanContext::Cleanup()
	{
		vmaDestroyAllocator(allocator);
		vkWaitForFences(device, 1, &GetCurrentFrame(frameNumber).renderFence, true, 1000000000);

		mainDeletionQueue.flush();

		swapchainDeletionQueue.flush();

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyDevice(device, nullptr);
		vkb::destroy_debug_utils_messenger(instance, debugMessenger);
		vkDestroyInstance(instance, nullptr);
	
	}
	//render loop
	void VulkanContext::SwapBuffers() //Draw function;
	{
		PROFILE_FUNCTION();

		CHECK_VULKAN(vkAcquireNextImageKHR(device, swapchain, 1000000000, GetCurrentFrame(frameNumber).presentSemaphore, nullptr, &swapchainImageIndex));

		CHECK_VULKAN(vkResetFences(device, 1, &GetCurrentFrame(frameNumber).renderFence));

		VkSubmitInfo submit = {};
		submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit.pNext = nullptr;

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		submit.pWaitDstStageMask = &waitStage;

		submit.waitSemaphoreCount = 1;
		submit.pWaitSemaphores = &GetCurrentFrame(frameNumber).presentSemaphore;

		submit.signalSemaphoreCount = 1;
		submit.pSignalSemaphores = &GetCurrentFrame(frameNumber).renderSemaphore;

		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &GetCurrentFrame(frameNumber).mainCommandBuffer;

		CHECK_VULKAN(vkQueueSubmit(graphicsQueue, 1, &submit, GetCurrentFrame(frameNumber).renderFence));

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;

		presentInfo.pSwapchains = &swapchain;
		presentInfo.swapchainCount = 1;

		presentInfo.pWaitSemaphores = &GetCurrentFrame(frameNumber).renderSemaphore;
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.pImageIndices = &swapchainImageIndex;

		CHECK_VULKAN(vkQueuePresentKHR(graphicsQueue, &presentInfo));

		vkWaitForFences(device, 1, &GetCurrentFrame(frameNumber).renderFence, true, 1000000000);
		CHECK_VULKAN(vkResetFences(device, 1, &GetCurrentFrame(frameNumber).renderFence));

		++frameNumber;
	}

	void VulkanContext::SetViewport(VkExtent2D extent)
	{
		windowExtent = extent;
		RebuildSwapchain();
	}

	void VulkanContext::SetVSync(bool enabled)
	{
		if (VSync != enabled)
		{
			VSync = enabled;
			RebuildSwapchain();
		}
	}

	void VulkanContext::RebuildSwapchain()
	{
		vkWaitForFences(device, 1, &GetCurrentFrame(frameNumber).renderFence, true, 1000000000);

		for (uint32_t i = 0; i < FRAME_OVERLAP; ++i)
		{
			CHECK_VULKAN(vkResetFences(device, 1, &GetCurrentFrame(i).renderFence));
		}
		
		swapchainDeletionQueue.flush();

		CreateSwapchain();
		InitCommands();
		InitDefaultRenderPass();
		InitFramebuffers();
		InitPipelines();
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

		CHECK_VULKAN(vkCreatePipelineLayout(device, &pipeline_layout_info, nullptr, &trianglePipelineLayout));

		VulkanPipelineBuilder pipelineBuilder;

		pipelineBuilder.m_ShaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, triangleVertShader));
		pipelineBuilder.m_ShaderStages.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, triangleFragShader));

		pipelineBuilder.m_VertexInputInfo = vkinit::vertex_input_state_create_info();
		pipelineBuilder.m_InputAssembly = vkinit::input_assembly_create_info(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		pipelineBuilder.m_Viewport.x = 0.0f;
		pipelineBuilder.m_Viewport.y = 0.0f;
		pipelineBuilder.m_Viewport.width = (float)windowExtent.width;
		pipelineBuilder.m_Viewport.height = (float)windowExtent.height;
		pipelineBuilder.m_Viewport.minDepth = 0.0f;
		pipelineBuilder.m_Viewport.maxDepth = 1.0f;

		pipelineBuilder.m_Scissor.offset = { 0, 0 };
		pipelineBuilder.m_Scissor.extent = windowExtent;

		pipelineBuilder.m_Rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

		pipelineBuilder.m_Multisampling = vkinit::multisampling_state_create_info();

		pipelineBuilder.m_ColorBlendAttachment = vkinit::color_blend_attachment_state();

		pipelineBuilder.m_PipelineLayout = trianglePipelineLayout;

		trianglePipeline = pipelineBuilder.BuildPipeline(device, renderPass);

		vkDestroyShaderModule(device, triangleVertShader, nullptr);
		vkDestroyShaderModule(device, triangleFragShader, nullptr);

		swapchainDeletionQueue.push_function([=]() {
			vkDestroyPipeline(device, trianglePipeline, nullptr);
			vkDestroyPipelineLayout(device, trianglePipelineLayout, nullptr);
			});
	}

	AllocatedBuffer VulkanContext::CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.pNext = nullptr;

		bufferInfo.size = allocSize;
		bufferInfo.usage = usage;

		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = memoryUsage;
		
		AllocatedBuffer newBuffer;

		CHECK_VULKAN(vmaCreateBuffer(allocator, &bufferInfo, &vmaallocInfo, &newBuffer.buffer, &newBuffer.allocation, nullptr));

		return newBuffer;
	}

	VkCommandBuffer VulkanContext::BeginImmediateExecute()
	{
		CHECK_VULKAN(vkResetFences(device, 1, &immediateFence));

		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = immediateCommandPool;
		allocateInfo.commandBufferCount = 1;

		VkCommandBuffer buffer;
		CHECK_VULKAN(vkAllocateCommandBuffers(device, &allocateInfo, &buffer));

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		CHECK_VULKAN(vkBeginCommandBuffer(buffer, &beginInfo));

		return buffer;
	}

	void VulkanContext::EndImmediateExecute(VkCommandBuffer buffer)
	{
		CHECK_VULKAN(vkEndCommandBuffer(buffer));

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &buffer;

		CHECK_VULKAN(vkQueueSubmit(graphicsQueue, 1, &submitInfo, immediateFence));
		
		vkWaitForFences(device, 1, &immediateFence, true, 1000000000);

		vkFreeCommandBuffers(device, immediateCommandPool, 1, &buffer);
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

		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			return false;
		}

		*outShaderModule = shaderModule;
		return true;
	}




	//this is for Dear ImGui integration
	InitInfo VulkanContext::CreateImGuiImplInfo()
	{
		InitInfo info{};
		info.Instance = instance;
		info.PhysicalDevice = physicalDevice;
		info.Device = device;
		info.QueueFamily = graphicsQueueFamily;
		info.Queue = graphicsQueue;
		info.PipelineCache = VK_NULL_HANDLE;
		info.DescriptorPool = descriptorPool;
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
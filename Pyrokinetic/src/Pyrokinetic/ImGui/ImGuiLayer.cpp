#include "pkpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "implot.h"

#include "ImGuiStyles.h"

#include "Pyrokinetic/Rendering/Renderer.h"

#define IMGUI_IMPL_API
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_vulkan.h"

#include "Pyrokinetic/Core/Application.h"

#include "Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <vulkan/vulkan.h>


namespace pk
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnAttach()
	{
		PROFILE_FUNCTION();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_viewportsNoMerge;

		// Setup Dear ImGui Style
		//ImGui::StyleColorsPKLight();
		ImGui::StyleColorsPKDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());


		// Setup Platform/Renderer Bindings

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			VulkanContext* context = dynamic_cast<VulkanContext*>(Renderer::GetContext());
			ImGui_ImplVulkan_InitInfo* info = (ImGui_ImplVulkan_InitInfo*) &(context->CreateImGuiImplInfo());
			ImGui_ImplGlfw_InitForVulkan(window, true);
			ImGui_ImplVulkan_Init(info, context->GetSwapchain().GetRenderPass()->GetVulkanRenderPass());
			
			
			auto cmd = context->GetDevice()->GetCommandBuffer(true);
			ImGui_ImplVulkan_CreateFontsTexture(cmd.buffer);
			context->GetDevice()->EndCommandBuffer(cmd, true);

			//clear font textures from cpu data
			ImGui_ImplVulkan_DestroyFontUploadObjects();
			
		}
		else if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
		{
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 410");
		}
	}

	void ImGuiLayer::OnDetach()
	{
		PROFILE_FUNCTION();


		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		}
		else if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
		}

		ImGui::DestroyContext();
		ImPlot::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.HandleEvent(e.IsInCategory(EventCategoryMouse) && io.WantCaptureMouse);
			e.HandleEvent(e.IsInCategory(EventCategoryKeyboard) && io.WantCaptureKeyboard);
		}
	}

	void ImGuiLayer::Begin()
	{
		PROFILE_FUNCTION();

		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		}
		else if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
		}
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		ImGui::Render();
		ImDrawData* drawData = ImGui::GetDrawData();
		if (RendererAPI::GetAPI() == RendererAPI::API::Vulkan)
		{

			
			VulkanContext* context = dynamic_cast<VulkanContext*>(Renderer::GetContext());
			auto cmd = context->GetDevice()->GetCommandBuffer(true);
			VkClearValue clearValue;
			clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };

			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = nullptr;
			renderPassBeginInfo.renderPass = context->GetSwapchain().GetRenderPass()->GetVulkanRenderPass();
			renderPassBeginInfo.renderArea.offset.x = 0;
			renderPassBeginInfo.renderArea.offset.y = 0;
			renderPassBeginInfo.renderArea.extent.width = context->GetSwapchain().GetWidth();
			renderPassBeginInfo.renderArea.extent.height = context->GetSwapchain().GetHeight();
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValue;
			renderPassBeginInfo.framebuffer = context->GetSwapchain().GetCurrentFramebuffer()->GetVulkanFramebuffer();
			vkCmdBeginRenderPass(cmd.buffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			ImGui_ImplVulkan_RenderDrawData(drawData, cmd.buffer);
			vkCmdEndRenderPass(cmd.buffer);
			context->GetDevice()->EndCommandBuffer(cmd, true);
		}
		else if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
		{
			ImGui_ImplOpenGL3_RenderDrawData(drawData);
		}

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

}
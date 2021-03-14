#include "Viewport2D.h"
#include "imgui.h"
#include "implot.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Pyrokinetic/Profiling/Instrumentor.h"

Viewport2D::Viewport2D()
	: Layer("2D Viewport"), m_CameraController(1280.0f/720.0f, true)
{
}
void Viewport2D::OnAttach()
{
	PROFILE_FUNCTION();

	m_Texture = pk::Texture2D::Create("assets/textures/checker.png");
	m_Spritesheet = pk::Texture2D::Create("assets/textures/RPGpack_sheet_2X.png");

	m_StairSprite = pk::SubTexture2D::CreateFromCoordinates(m_Spritesheet, { 1, 3 }, { 128, 128 }, { 1, 1 });
	m_LargeTree = pk::SubTexture2D::CreateFromCoordinates(m_Spritesheet, { 1, 1 }, { 128, 128 }, { 1, 2 });

	pk::FramebufferSpecification spec;
	spec.width = 1280;
	spec.height = 720;

	m_Framebuffer = pk::Framebuffer::Create(spec);
}

void Viewport2D::OnDetach()
{
	PROFILE_FUNCTION();
}

void Viewport2D::OnUpdate(pk::Timestep timestep)
{
	PROFILE_FUNCTION();
	{
		PROFILE_SCOPE("OrthographicCamera::OnUpdate()");
		m_CameraController.OnUpdate(timestep);
	}
	pk::Renderer2D::ResetStats();
	{
		PROFILE_SCOPE("Viewport2D::RenderPreparation()");
		m_Framebuffer->Bind();
		pk::RenderCommand::Begin();
		pk::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1 });
		pk::RenderCommand::Clear();
	}

	{
		PROFILE_SCOPE("Viewport2D::RenderScene()");
		pk::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (int i = 0; i < 100; ++i)
		{
			for (int j = 0; j < 100; ++j)
			{
				pk::Renderer2D::DrawQuad({ 1.41f * i,  1.41f * j }, { 1, 1 }, { (float)j / 100.0f , (float)i / 100.0f , 0.75f , 1.0f }, 45.0f);
			}
		}
		pk::Renderer2D::EndScene();

		pk::RenderCommand::End();

		m_Framebuffer->Unbind();
	}
}

void Viewport2D::OnImGuiRender()
{
	PROFILE_FUNCTION();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Open"))
			{
				ImGui::Text("Project");
				ImGui::Text("Scene");
				ImGui::Text("Asset");

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	//auto stats = pk::Renderer2D::GetStats();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Scene");

	pk::Application::Get().GetImGuiLayer()->SetBlockEvents(!(ImGui::IsWindowHovered()));

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	if (m_ViewportSize != *(glm::vec2*)&viewportPanelSize)
	{
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		m_Framebuffer->Resize((uint32_t) m_ViewportSize.x, (uint32_t) m_ViewportSize.y);
		m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);
	}
	/*
	ImGui::Text("Draw Calls: %d", stats.drawCalls);
	ImGui::Text("Quads: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());*/

	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	ImGui::End();
	ImGui::PopStyleVar(ImGuiStyleVar_WindowPadding);

	ImGui::Begin("Scene Hierarchy");
	ImGui::End();

	ImGui::Begin("Log");
	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::End();

	ImGui::ShowDemoWindow();
}

void Viewport2D::OnEvent(pk::Event& e)
{
	m_CameraController.OnEvent(e);
}
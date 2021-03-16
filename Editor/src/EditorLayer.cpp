#include "EditorLayer.h"

#include "Pyrokinetic/Profiling/Instrumentor.h"

#include "imgui.h"
#include "implot.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace pk
{
	EditorLayer::EditorLayer()
		: Layer("2D Viewport"), m_CameraController(1280.0f / 720.0f, true)
	{
	}
	void EditorLayer::OnAttach()
	{
		PROFILE_FUNCTION();

		m_Texture = Texture2D::Create("assets/textures/checker.png");
		m_Spritesheet = Texture2D::Create("assets/textures/RPGpack_sheet_2X.png");

		m_StairSprite = SubTexture2D::CreateFromCoordinates(m_Spritesheet, { 1, 3 }, { 128, 128 }, { 1, 1 });
		m_LargeTree = SubTexture2D::CreateFromCoordinates(m_Spritesheet, { 1, 1 }, { 128, 128 }, { 1, 2 });

		FramebufferSpecification spec;
		spec.width = 1280;
		spec.height = 720;

		m_Framebuffer = Framebuffer::Create(spec);

		m_ActiveScene = std::make_shared<Scene>();

		m_SquareEntity = m_ActiveScene->CreateEntity("Square");
		m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f });

		m_CameraEntity = m_ActiveScene->CreateEntity("Orthographic Camera");
		auto& c = m_CameraEntity.AddComponent<CameraComponent>();
		c.Primary = true;

		m_ClipspaceCameraEntity = m_ActiveScene->CreateEntity("Clip Space Camera");
		auto& c2  = m_ClipspaceCameraEntity.AddComponent<CameraComponent>();
		c2.Primary = false;
	}

	void EditorLayer::OnDetach()
	{
		PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep t)
	{
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.width != m_ViewportSize.x || spec.height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}




		m_CameraController.OnUpdate(t);

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1 });
		RenderCommand::Clear();

		m_ActiveScene->OnUpdate(t);

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
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

		//auto stats = Renderer2D::GetStats();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene");

		Application::Get().GetImGuiLayer()->SetBlockEvents(!(ImGui::IsWindowHovered()));

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *(glm::vec2*)&viewportPanelSize)
		{
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
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
		ImGui::Separator();
		ImGui::DragFloat3("Camera Transform", glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Transform[3]));
		if (ImGui::Checkbox("Clipspace Camera", &m_PrimaryCamera))
		{
			m_CameraEntity.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
			m_ClipspaceCameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
		}
		ImGui::Separator();
		ImGui::End();

		ImGui::ShowDemoWindow();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}
}
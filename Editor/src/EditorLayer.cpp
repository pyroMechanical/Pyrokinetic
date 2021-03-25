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

		m_ViewportSize = { spec.width, spec.height };

		m_Framebuffer = Framebuffer::Create(spec);

		m_ActiveScene = std::make_shared<Scene>();

		m_SquareEntity = m_ActiveScene->CreateEntity("Sprite");
		m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 1.0f, 1.0f }, m_Texture);
		m_SquareEntity.GetComponent<TransformComponent>().Translation = { -3.0f, 0.0f, 0.0f };

		m_SquareEntity2 = m_ActiveScene->CreateEntity("Sprite 2");
		m_SquareEntity2.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f }, m_Texture);
		m_SquareEntity2.GetComponent<TransformComponent>().Translation = { 1.5f, 0.0f, 0.0f };

		m_SquareEntity3 = m_ActiveScene->CreateEntity("Sprite 3");
		m_SquareEntity3.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 1.0f, 1.0f }, m_Texture);
		m_SquareEntity3.GetComponent<TransformComponent>().Translation = { 1.5f, 0.0f, 0.0f };

		m_SquareEntity4 = m_ActiveScene->CreateEntity("Sprite 4");
		m_SquareEntity4.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f }, m_Texture);
		m_SquareEntity4.GetComponent<TransformComponent>().Translation = { 1.5f, 0.0f, 0.0f };

		m_SquareEntity5 = m_ActiveScene->CreateEntity("Sprite 5");
		m_SquareEntity5.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f }, m_Texture);
		m_SquareEntity5.GetComponent<TransformComponent>().Translation = { 1.5f, 0.0f, 0.0f };

		m_SquareEntity.AddChild(m_SquareEntity2);
		m_SquareEntity2.AddChild(m_SquareEntity3);
		m_SquareEntity3.AddChild(m_SquareEntity4);
		m_SquareEntity4.AddChild(m_SquareEntity5);

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera");
		auto& c = m_CameraEntity.AddComponent<CameraComponent>();
		c.Primary = true;
		c.FixedAspectRatio = false;
		m_CameraEntity.GetComponent<TransformComponent>().Translation = { 0.0f, 0.0f, 6.0f };

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate()
			{
			}

			void OnDestroy()
			{
			}

			void OnUpdate(Timestep t)
			{
				auto& transform = GetComponent<TransformComponent>();
				float speed = 5.0f;

				if (Input::IsKeyPressed(PK_KEY_W))
				{
					transform.Translation.y += speed * t;
				}
				if (Input::IsKeyPressed(PK_KEY_S))
				{
					transform.Translation.y -= speed * t;
				}
				if (Input::IsKeyPressed(PK_KEY_A))
				{
					transform.Translation.x -= speed * t;
				}
				if (Input::IsKeyPressed(PK_KEY_D))
				{
					transform.Translation.x += speed * t;
				}
			}
		};
		
		//m_CameraEntity.AddComponent<CPPScriptComponent>().Bind<CameraController>();

		m_SceneHierarchy.SetContext(m_ActiveScene);
		m_Properties.SetContext(m_ActiveScene);
		m_SceneHierarchy.SetPropertiesPanel(&m_Properties);
	}

	void EditorLayer::OnDetach()
	{
		PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep t)
	{
		FramebufferSpecification spec = m_Framebuffer->GetSpecification();
		if ((m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f) && (spec.width != (uint32_t)m_ViewportSize.x || spec.height != (uint32_t)m_ViewportSize.y))
		{
			ResizeViewport();
		}

		m_CameraController.OnUpdate(t);

		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
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

		m_SceneHierarchy.OnImGuiRender();
		m_Properties.OnImGuiRender();

		auto stats = Renderer2D::GetStats();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene");

		Application::Get().GetImGuiLayer()->SetBlockEvents(!(ImGui::IsWindowHovered()));
		ImVec2 v = ImGui::GetContentRegionAvail();
		glm::vec2 viewportPanelSize = { v.x, v.y };
		if (m_ViewportSize != viewportPanelSize && viewportPanelSize.x > 0.0f && viewportPanelSize.y > 0.0f)
		{
			m_ViewportSize = viewportPanelSize;
		}

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar(ImGuiStyleVar_WindowPadding);

		ImGui::Begin("Log");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Tris: %d", stats.quadCount*2);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::End();

		//ImGui::ShowDemoWindow();

		Renderer2D::ResetStats();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}

	void EditorLayer::ResizeViewport()
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}
}
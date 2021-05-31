#include "EditorLayer.h"

#include "Pyrokinetic/Profiling/Instrumentor.h"

#include "imgui.h"
#include "implot.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace pk
{
	EditorLayer::EditorLayer()
		: Layer("2D Viewport")
	{
	}
	void EditorLayer::OnAttach()
	{
		PROFILE_FUNCTION();

		m_RenderPass = Renderer::GetRenderPass("SceneView");

		FramebufferSpecification spec;
		spec.width = 1280;
		spec.height = 720;
		spec.renderPass = m_RenderPass;

		m_Framebuffer = Framebuffer::Create(spec);

		m_ViewportSize = { spec.width, spec.height };

		m_ActiveScene = std::make_shared<Scene>();

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

		Renderer::BeginRenderPass(m_RenderPass);

		m_ActiveScene->OnUpdate(t);

		Renderer::EndRenderPass();

		Renderer2D::Flush();
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
					if (ImGui::MenuItem("Scene"))
					{
						auto name = util::FileBrowser::Open("Pyrokinetic file (.pk)\0 *.pk\0");
						if (!name.empty())
						{
							m_ActiveScene = std::make_shared<Scene>();
							m_SceneHierarchy.SetContext(m_ActiveScene);
							m_Properties.SetContext(m_ActiveScene);
							SceneSerializer serializer(m_ActiveScene);
							serializer.DeserializeText(name);
							ResizeViewport();
						}
					}
					if (ImGui::BeginMenu("Asset"))
					{
						if (ImGui::MenuItem("Texture"))
						{
							auto path = util::FileBrowser::Open("Image file (*.png, *.jpg)\0 *.png\0 *.jpg\0");
							AssetManager::CreateAsset(path);
						}
						ImGui::EndMenu();
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Save"))
				{
					if (ImGui::MenuItem("Scene"))
					{
						auto name = util::FileBrowser::Save("Pyrokinetic file (.pk)\0 *.pk\0");

						SceneSerializer serializer(m_ActiveScene);
						serializer.SerializeText(name);
					}
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
			PK_CORE_INFO("Resizing viewport: {0}, {1} to {2}, {3}", m_ViewportSize.x, m_ViewportSize.y, viewportPanelSize.x, viewportPanelSize.y);
			m_ViewportSize = viewportPanelSize;
		}

		ImGui::Image(m_Framebuffer->GetColorAttachment(0), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar(ImGuiStyleVar_WindowPadding);

		ImGui::Begin("Log");
		ImGui::Text("Draw Calls: %d", stats.drawCalls);
		ImGui::Text("Tris: %d", stats.quadCount*2);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::End();

		Renderer2D::ResetStats();
	}

	void EditorLayer::OnEvent(Event& e)
	{
	}

	void EditorLayer::ResizeViewport()
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}
}
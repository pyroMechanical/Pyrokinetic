#include "Properties.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace pk
{
	PropertiesPanel::PropertiesPanel(const std::shared_ptr<Scene>& context)
	{
		SetContext(context);
	}

	void PropertiesPanel::SetContext(const std::shared_ptr<Scene>& context)
	{
		m_Context = context;
	}

	void PropertiesPanel::OnImGuiRender()
	{
		ImGui::Begin("Properties");

		DrawComponents();
		
		if (m_SelectedEntity)
		{
			if (ImGui::Button("Add Component"))
				ImGui::OpenPopup("component_popup");

			if (ImGui::BeginPopup("component_popup"))
			{
				if (!m_SelectedEntity.HasComponent<CameraComponent>())
					if (ImGui::Selectable("Camera"))
					{
						m_SelectedEntity.AddComponent<CameraComponent>();
						ImGui::CloseCurrentPopup();
					}

				if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
					if (ImGui::Selectable("Sprite"))
					{
						m_SelectedEntity.AddComponent<SpriteRendererComponent>();
						ImGui::CloseCurrentPopup();
					}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}

	void PropertiesPanel::DrawComponents()
	{
		if (m_SelectedEntity)
		{
			if(m_SelectedEntity.HasComponent<TagComponent>())
			{
				auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;

				//TODO: rewrite to support wide characters
				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy_s(buffer, sizeof(buffer), tag.c_str());
				if (ImGui::InputText("Name", buffer, sizeof(buffer)))
				{
					tag = std::string(buffer);
				}
			}

			if (m_SelectedEntity.HasComponent<TransformComponent>())
			{
				
				if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
				{
					auto& transform = m_SelectedEntity.GetComponent<TransformComponent>();

					if (ImGui::DragFloat3("Position", glm::value_ptr(transform.Translation), 0.05f)) transform.Dirty = true;
					if (ImGui::DragFloat3("Rotation", glm::value_ptr(transform.EulerAngles), 1.0f))
					{
						transform.RecalculateQuaternion();
						transform.Dirty = true;
					}
					if (ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.05f)) transform.Dirty = true;

					/*ImGui::Text("Global Transform");

					if(transform.Dirty = true)
					{
						transform.RecalculateMatrix();
					}
					
					glm::mat4 global = transform.GetWorldMatrix();
					glm::vec3 t;
					glm::quat r;
					glm::vec3 s;
					glm::vec3 sk;
					glm::vec4 p;
					glm::decompose(global, s, r, t, sk, p);
					glm::vec3 ea = glm::degrees(glm::eulerAngles(glm::conjugate(r)));

					ImGui::DragFloat3("Global Position", glm::value_ptr(t), 1);
					ImGui::DragFloat3("Global Rotation", glm::value_ptr(ea), 1);
					ImGui::DragFloat3("Global Scale", glm::value_ptr(s), 1);
					ImGui::DragFloat3("Global Skew", glm::value_ptr(sk), 1);
					ImGui::DragFloat4("Global Perspective", glm::value_ptr(p), 1);*/

					ImGui::TreePop();
				}
			}

			if (m_SelectedEntity.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite"))
				{
					auto& sprite = m_SelectedEntity.GetComponent<SpriteRendererComponent>();
					auto& texture = sprite.Texture;

					ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color));
					
					ImGui::Text("Texture:");
					ImGui::SameLine();
					
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.3f, 0.3f, 0.3f, 1.0f});
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.35f, 0.35f, 0.35f, 1.0f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });

					bool pressed = false;

					if (texture)
					{
						glm::vec2 min = texture->GetMin();
						glm::vec2 max = texture->GetMax();
						glm::vec2 ratio = texture->GetTextureRatio();

						pressed = ImGui::ImageButton(texture->GetTexture()->GetImGuiTexture(), ImVec2{ ratio.x * 100.0f, ratio.y * 100.0f }, ImVec2{ min.x, max.y }, ImVec2{ max.x, min.y }, 5, ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

					}
					else
					{
						pressed = ImGui::Button("+ Add Texture", ImVec2{ 110.0f, 110.0f });
					}

					if(pressed)
					{
						std::string path = util::FileBrowser::Open("Image file (*.png, *.jpg)\0*.png\0 *.jpg\0");
						if (!path.empty())
							sprite.Texture = std::dynamic_pointer_cast<SubTexture2D>(AssetManager::GetAsset(AssetManager::CreateAsset(path)));
					}
					
					ImGui::PopStyleColor(3);
					
					ImGui::TreePop();
				}
			}

			if (m_SelectedEntity.HasComponent<CameraComponent>())
			{

				if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
				{
					auto& cameraComponent = m_SelectedEntity.GetComponent<CameraComponent>();
					auto& camera = cameraComponent.Camera;
					const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
					const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];

					if (ImGui::BeginCombo("Projection Type", currentProjectionTypeString))
					{
						for (int i = 0; i < 2; ++i)
						{
							bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
							if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
							{
								currentProjectionTypeString = projectionTypeStrings[i];
								camera.SetProjectionType(i);
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();

						}
						ImGui::EndCombo();
					}

					if(camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
					{
						float perspectiveFOV = camera.GetPerspectiveFOV();
						if (ImGui::DragFloat("Vertical FOV", &perspectiveFOV))
							camera.SetPerspectiveFOV(perspectiveFOV);
						float perspectiveNear = camera.GetPerspectiveNearClip();
						if (ImGui::DragFloat("Near Plane", &perspectiveNear))
							camera.SetPerspectiveNearClip(perspectiveNear);
						bool infinite = camera.IsFarPlaneInfinite();
						if(ImGui::Checkbox("Infinite Far Plane", &infinite))
							camera.SetFarPlaneInfinite(infinite);
						if (infinite)
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						}
							float perspectiveFar = camera.GetPerspectiveFarClip();
							if (ImGui::DragFloat("Far Plane", &perspectiveFar))
								camera.SetPerspectiveFarClip(perspectiveFar);
						if (infinite)
						{
							ImGui::PopItemFlag();
							ImGui::PopStyleVar();
						}
						
					}

					if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
					{
						float orthoSize = camera.GetOrthographicSize();
						if (ImGui::DragFloat("Size", &orthoSize)) 
							camera.SetOrthographicSize(orthoSize);
						float orthoNear = camera.GetOrthographicNearClip();
						if (ImGui::DragFloat("Near Plane", &orthoNear))
							camera.SetOrthographicNearClip(orthoNear);
						float orthoFar = camera.GetOrthographicFarClip();
						if (ImGui::DragFloat("Far Plane", &orthoFar))
							camera.SetOrthographicFarClip(orthoFar);
					}

					ImGui::TreePop();
				}
			}
		}

	}
}
#include "Properties.h"

#include <imgui.h>

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

					ImGui::DragFloat3("Position", glm::value_ptr(transform.Translation), 0.05f);
					if (ImGui::DragFloat3("Rotation", glm::value_ptr(transform.EulerAngles), 1.0f))
					{
						transform.RecalculateQuaternion();
					}
					ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.05f);
					ImGui::TreePop();
				}
			}

			if (m_SelectedEntity.HasComponent<SpriteRendererComponent>())
			{

				if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite"))
				{
					auto& sprite = m_SelectedEntity.GetComponent<SpriteRendererComponent>();

					Texture2D* texture = sprite.Texture.get();

					ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color));
					if(texture)
					ImGui::Image((void*)texture->GetRendererID(), ImVec2{ 64.0f, 64.0f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
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
						if(!infinite)
						{
							float perspectiveFar = camera.GetPerspectiveFarClip();
							if (ImGui::DragFloat("Far Plane", &perspectiveFar))
								camera.SetPerspectiveFarClip(perspectiveFar);
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
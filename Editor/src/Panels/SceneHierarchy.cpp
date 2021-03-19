#include "SceneHierarchy.h"

#include "Pyrokinetic/Scene/Components.h"

#include <imgui.h>

namespace pk
{
	SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		
		m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = {};
			if (p_Properties != nullptr) p_Properties->ClearSelectedEntity();
		}

		// not hovering over an entity
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty"))
				m_Context->CreateEntity("GameObject");

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		if (tag != "Entity")
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			m_SelectionContext == entity ? flags |= ImGuiTreeNodeFlags_Selected : 0;
			bool opened = ImGui::TreeNodeEx((void*)(unsigned int)(uint32_t)entity, flags, tag.c_str());
			if (ImGui::IsItemClicked())
			{
				m_SelectionContext = entity;
				if(p_Properties != nullptr) p_Properties->SetSelectedEntity(entity);
			}
			bool EntityDeleted = false;
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete Object"))
				{
					EntityDeleted = true;
				}

				ImGui::EndPopup();
			}

			if (opened)
			{
				ImGui::TreePop();
			}

			if (EntityDeleted)
			{
				m_Context->DestroyEntity(entity);
				if(m_SelectionContext == entity)
				{
					m_SelectionContext = {};
					if (p_Properties != nullptr) p_Properties->ClearSelectedEntity();
				}
			}
		}
	}
}
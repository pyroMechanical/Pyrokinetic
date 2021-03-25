#include "SceneHierarchy.h"

#include "Pyrokinetic/Scene/Components.h"

#include <imgui.h>

#include <imgui_internal.h>

#include <queue>

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



		auto group = m_Context->m_Registry.group<TagComponent>(entt::exclude<ChildComponent>);
		for (auto each : group)
		{
			Entity entity{ each, m_Context.get() };
			DrawEntityNode(entity);
		}

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

		if (ImGui::IsWindowHovered())
		{
			ImGui::PushStyleColor(ImGuiCol_DragDropTarget, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
			ImGui::PushID("Dummy");
			ImGui::Dummy(ImVec2(0.0f, 0.0f));
			ImGui::PopID();

			if (ImGui::BeginDragDropTargetCustom(ImRect(ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax()), ImGui::GetID("Dummy")))
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITYNODE"))
				{
					PK_CORE_ASSERT(payload->DataSize == sizeof(Entity));
					Entity entityValue = *(Entity*)payload->Data;
					m_Context->RemoveChild(entityValue);
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::PopStyleColor();
		}

		ImGui::End();
	}

	//TODO: sort this nonsense out.

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{

		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
		m_SelectionContext == entity ? flags |= ImGuiTreeNodeFlags_Selected : 0;
		(!entity.HasComponent<ParentComponent>() || entity.GetComponent<ParentComponent>().First == entt::null) ? flags |= ImGuiTreeNodeFlags_Leaf : 0;
		bool opened = ImGui::TreeNodeEx((void*)(unsigned int)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
			if (p_Properties != nullptr) p_Properties->SetSelectedEntity(entity);
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

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITYNODE", &entity, sizeof(Entity));
			ImGui::Text(tag.c_str());
			ImGui::EndDragDropSource();
		}
		bool isParent;
		if (ImGui::GetDragDropPayload())
		{
			const ImGuiPayload* payload = ImGui::GetDragDropPayload();
			if (payload->DataSize == sizeof(Entity))
			{
				Entity entityValue = *(Entity*)payload->Data;
				isParent = m_Context->IsParent(entity, entityValue);
			}
		}
		if (!isParent)
		{
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITYNODE"))
				{
					PK_CORE_ASSERT(payload->DataSize == sizeof(Entity));
					PK_CORE_INFO("{0}, {1}", (uint32_t)entity, (uint32_t) * (Entity*)payload->Data);
					Entity entityValue = *(Entity*)payload->Data;
					entity.AddChild(entityValue);
				}
				ImGui::EndDragDropTarget();
			}
		}

		//TODO:: make it so removing entities by putting them in the main hierarchy window works

		//if(!ImGui::IsDragDropPayloadBeingAccepted())

		if (opened)
		{

			if (entity.HasComponent<ParentComponent>())
			{
				auto& parent = entity.GetComponent<ParentComponent>();
				Entity childEntity{ parent.First, m_Context.get() };

				for (int i = 0; i < parent.Children; ++i)
				{
					DrawEntityNode(childEntity);
					childEntity = Entity{ childEntity.GetComponent<ChildComponent>().Next, m_Context.get() };
				}
			}

			ImGui::TreePop();
		}

		if (EntityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
			{
				m_SelectionContext = {};
				if (p_Properties != nullptr) p_Properties->ClearSelectedEntity();
			}
		}
	}
}
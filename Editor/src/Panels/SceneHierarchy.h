#pragma once
#include "Pyrokinetic.h"

#include "Properties.h"

#include "Pyrokinetic/Scene/Scene.h"
#include "Pyrokinetic/Scene/Entity.h"

#include <memory>

namespace pk
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const std::shared_ptr<Scene>& context);

		void SetContext(const std::shared_ptr<Scene>& context);

		void OnImGuiRender();

		void SetPropertiesPanel(PropertiesPanel* p) { p_Properties = p; }

	private:
		void DrawEntityNode(Entity entity);
	private:
		std::shared_ptr<Scene> m_Context;
		Entity m_SelectionContext;
		PropertiesPanel* p_Properties = nullptr;
	};
}
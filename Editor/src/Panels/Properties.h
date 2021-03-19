#pragma once
#include "Pyrokinetic.h"

namespace pk
{
	class PropertiesPanel
	{
	public:
		PropertiesPanel() = default;
		PropertiesPanel(const std::shared_ptr<Scene>& context);

		void SetContext(const std::shared_ptr<Scene>& context);

		void OnImGuiRender();

		void SetSelectedEntity(Entity& e) { m_SelectedEntity = e; }
		void ClearSelectedEntity() { m_SelectedEntity = {}; }
	private:
		void DrawComponents();
	private:
		std::shared_ptr<Scene> m_Context;
		Entity m_SelectedEntity;
	};
}
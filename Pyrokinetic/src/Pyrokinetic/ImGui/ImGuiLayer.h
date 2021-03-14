#pragma once
#include "Pyrokinetic/Core/Layer.h"

#include "Pyrokinetic/Events/MouseEvent.h"
#include "Pyrokinetic/Events/KeyEvent.h"
#include "Pyrokinetic/Events/ApplicationEvent.h"


namespace pk 
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& event) override;
		void Begin();
		void End();

		void SetBlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = false;
		float m_Time = 0.0f;
	};
}

#pragma once
#include "Pyrokinetic/Core/Layer.h"

#include "Pyrokinetic/Events/MouseEvent.h"
#include "Pyrokinetic/Events/KeyEvent.h"
#include "Pyrokinetic/Events/ApplicationEvent.h"


namespace Pyrokinetic 
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};
}

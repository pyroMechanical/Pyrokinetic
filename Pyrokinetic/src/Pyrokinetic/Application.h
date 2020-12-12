#pragma once

#include "Core.h"
#include "Pyrokinetic/Events/ApplicationEvent.h"
#include "Events/Event.h"

#include "Window.h"

namespace Pyrokinetic
{
	class PK_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		
		void OnEvent(Event& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	//To be defined in client
	Application* CreateApplication();
}

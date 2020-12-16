#pragma once
#include "Core.h"

#include "Window.h"
#include "Pyrokinetic/LayerStack.h"
#include "Pyrokinetic/Events/Event.h"
#include "Pyrokinetic/Events/ApplicationEvent.h"

namespace Pyrokinetic
{
	class PK_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};

	//To be defined in client
	Application* CreateApplication();
}

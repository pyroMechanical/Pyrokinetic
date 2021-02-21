#pragma once
#include "Core.h"
#include "Pyrokinetic/Core/Timestep.h"

#include "Window.h"
#include "Pyrokinetic/Core/LayerStack.h"
#include "Pyrokinetic/Events/Event.h"
#include "Pyrokinetic/Events/ApplicationEvent.h"

#include "Pyrokinetic/ImGui/ImGuiLayer.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Pyrokinetic/Rendering/Buffer.h"
#include "Pyrokinetic/Rendering/VertexArray.h"

#include "Pyrokinetic/Rendering/Camera.h"

namespace Pyrokinetic
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static Application& Get() { return *s_Instance; }
		Window& GetWindow() { return *m_Window; }
		Window* GetWindowPtr() { return m_Window.get(); }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	//To be defined in client
	Application* CreateApplication();
}

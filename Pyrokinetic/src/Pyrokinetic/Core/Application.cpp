#pragma once
#include "pkpch.h"
#include "Application.h"
#include "Input.h"
#include <glm/glm.hpp>
#include "Pyrokinetic/Rendering/Renderer.h"
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>


namespace pk
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		PROFILE_FUNCTION();

		PK_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		m_Window->SetVSync(true);
		Renderer::Init();
#ifndef PK_VULKAN_SUPPORTED
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
#endif
	}

	Application::~Application()
	{
		PROFILE_FUNCTION();
	}

	void Application::PushLayer(Layer* layer)
	{
		PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
		//PK_CORE_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.IsHandled())
				break;
		}
	}

	void Application::Run()
	{
		PROFILE_FUNCTION();

		float degrees = 0;
		while (m_Running)
		{
			PROFILE_SCOPE("Run Loop")
			float time = (float) glfwGetTime(); // Platform::GetTime
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;
			if (!m_Minimized)
			{
				{
					PROFILE_SCOPE("LayerStack Update");
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}
				{
					PROFILE_SCOPE("ImGuiStack Update");
					m_ImGuiLayer->Begin();
					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
					m_ImGuiLayer->End();
				}
			}
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		PROFILE_FUNCTION();

		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

}
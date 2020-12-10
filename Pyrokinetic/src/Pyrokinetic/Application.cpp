#include "pkpch.h"

#include "Application.h"

#include "Pyrokinetic/Events/ApplicationEvent.h"
#include "Pyrokinetic/Log.h"


namespace Pyrokinetic
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		PK_TRACE(e);
		while (true);
	}
}
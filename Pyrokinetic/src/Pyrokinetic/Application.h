#pragma once

#include "Core.h"

namespace Pyrokinetic
{
	class PK_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	//To be defined in client
	Application* CreateApplication();
}

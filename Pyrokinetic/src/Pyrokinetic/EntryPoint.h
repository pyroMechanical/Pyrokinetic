#pragma once
#include "pyrokinetic.h"

#ifdef PK_PLATFORM_WINDOWS

extern Pyrokinetic::Application* Pyrokinetic::CreateApplication();

int main(int argc, char** argv)
{
	Pyrokinetic::Log::Init();
	PK_CORE_WARN("Initialized Logger");
	PK_INFO("Initialized Client Logger");

	auto app = Pyrokinetic::CreateApplication();
	app->Run();
	delete app;
}

#endif
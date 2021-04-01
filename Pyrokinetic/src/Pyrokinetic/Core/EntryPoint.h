#pragma once
#include "Pyrokinetic.h"

#ifdef PK_PLATFORM_WINDOWS

extern pk::Application* pk::CreateApplication();

int main(int argc, char** argv)
{
	pk::Log::Init();
	PROFILE_BEGIN_SESSION("Startup", "Profile-Startup.json");

	auto app = pk::CreateApplication();
	PROFILE_END_SESSION();
	PROFILE_BEGIN_SESSION("Startup", "Profile-Runtime.json");
	app->Run();
	PROFILE_END_SESSION();
	PROFILE_BEGIN_SESSION("Startup", "Profile-Shutdown.json");
	delete app;
	PROFILE_END_SESSION();
	std::cin.get();
}

#endif


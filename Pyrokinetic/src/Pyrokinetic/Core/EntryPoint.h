#pragma once
#include "Pyrokinetic.h"

#ifdef PK_PLATFORM_WINDOWS

extern Pyrokinetic::Application* Pyrokinetic::CreateApplication();

int main(int argc, char** argv)
{
	Pyrokinetic::Log::Init();
	PROFILE_BEGIN_SESSION("Startup", "Profile-Startup.json");

	auto app = Pyrokinetic::CreateApplication();
	PROFILE_END_SESSION();
	PROFILE_BEGIN_SESSION("Startup", "Profile-Runtime.json");
	app->Run();
	PROFILE_END_SESSION();
	PROFILE_BEGIN_SESSION("Startup", "Profile-Shutdown.json");
	delete app;
	PROFILE_END_SESSION();
#ifdef PK_DEBUG
	std::cin.get();
#endif
}

#endif


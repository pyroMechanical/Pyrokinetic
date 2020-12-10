#pragma once

#ifdef PK_PLATFORM_WINDOWS

extern Pyrokinetic::Application* Pyrokinetic::CreateApplication();

int main(int argc, char** argv)
{
	Pyrokinetic::Log::Init();
	PK_CORE_WARN("Initialized Logger");
	int a = 42;
	PK_INFO("Initialized Client Logger. Var = {0}", a);

	auto app = Pyrokinetic::CreateApplication();
	app->Run();
	delete app;
}

#endif
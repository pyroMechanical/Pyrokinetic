#pragma once

#ifdef PK_PLATFORM_WINDOWS

extern Pyrokinetic::Application* Pyrokinetic::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Pyrokinetic::CreateApplication();
	app->Run();
	delete app;
}

#endif
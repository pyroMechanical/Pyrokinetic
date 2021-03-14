#include "pkpch.h"
#include "Window.h"

#include "Platform/Windows/WindowsWindow.h"

namespace pk
{
	
		Window* Window::Create(const WindowProps & props)
		{
#ifdef PK_PLATFORM_WINDOWS
			return new WindowsWindow(props);
#endif
#ifdef PK_PLATFORM_MACOS
			return new MacWindow(props);
#endif
#ifdef PK_PLATFORM_LINUX
			return new LinuxWindow(props);
#endif
		}
}
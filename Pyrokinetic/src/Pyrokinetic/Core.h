#pragma once

#ifdef PK_PLATFORM_WINDOWS
	#ifdef PK_BUILD_DLL
		#define PK_API __declspec(dllexport)
	#else
		#define PK_API __declspec(dllimport)
	#endif
#else
	#error OS unsupported!
#endif
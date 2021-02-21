#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Pyrokinetic/Core/Log.h"

//#include "Pyrokinetic/Profiling/Instrumentor.h"

#define PROFILE_BEGIN_SESSION(name, filepath)
#define PROFILE_END_SESSION()
#define PROFILE_FUNCTION()
#define PROFILE_SCOPE(name)

#define PK_VULKAN_SUPPORTED

//Platforms
#ifdef PK_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
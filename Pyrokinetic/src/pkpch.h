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

#include "Pyrokinetic/Core/Core.h"

#include "Pyrokinetic/Core/Log.h"



#include "Pyrokinetic/Profiling/Instrumentor.h"

//#define PK_VULKAN_SUPPORTED

//Platforms
#ifdef PK_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
#ifdef PK_PLATFORM_MACOS

#endif
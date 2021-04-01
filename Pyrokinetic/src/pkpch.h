#pragma once
#define IM_ASSERT(_EXPR) { if(!(_EXPR)) { PK_ERROR("ImGui Assert Failed: {0}", _EXPR); __debugbreak(); } }
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

//Platforms
#ifdef PK_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
#ifdef PK_PLATFORM_MACOS

#endif
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
#ifdef PK_DEBUG
#include "Pyrokinetic/Profiling/Instrumentor.h"
#endif
//Platforms
#ifdef PK_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
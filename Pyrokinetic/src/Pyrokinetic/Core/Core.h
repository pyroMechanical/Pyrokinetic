#pragma once
#include <memory>

#ifdef _WIN32
#ifdef _WIN64
#define PK_PLATFORM_WINDOWS
#else
#error "x86 not supported"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS sim not supported"
#elif TARGET_OS_IPHONE == 1
#define PK_PLATFORM_IOS
#error "IOS not supported"
#elif TARGET_OS_MAC == 1
#define PK_PLATFORM_MACOS
#else
#error "Unknown apple platform"
#endif
#elif defined(__ANDROID__)
#define PK_PLATFORM_ANDROID
#error "Android not supported"
#elif defined(__linux__)
#define PK_PLATFORM_LINUX
#error "Linux not supported"
#else
#error "Unknown platform"
#endif

#ifdef PK_DEBUG
#define PK_ENABLE_ASSERTS
#endif

#ifdef PK_ENABLE_ASSERTS
#define PK_ASSERT(x, ...) { if(!(x)) { PK_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define PK_CORE_ASSERT(x, ...) { if(!(x)) { PK_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define PK_ASSERT(x, ...)
#define PK_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define PK_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)


namespace Pyrokinetic
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
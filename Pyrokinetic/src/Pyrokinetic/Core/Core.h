#pragma once
#include <memory>

#ifndef PK_PLATFORM
#ifdef _WIN32
#ifdef _WIN64
#define PK_PLATFORM_WINDOWS
#define PK_PLATFORM PK_PLATFORM_WINDOWS
#else
#error "x86 not supported"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS sim not supported"
#elif TARGET_OS_IPHONE == 1
#define PK_PLATFORM_IOS
#define PK_PLATFORM PK_PLATFORM_IOS
#error "IOS not supported"
#elif TARGET_OS_MAC == 1
#define PK_PLATFORM_MACOS
#define PK_PLATFORM PK_PLATFORM_MACOS
#else
#error "Unknown apple platform"
#endif
#elif defined(__ANDROID__)
#define PK_PLATFORM_ANDROID
#define PK_PLATFORM PK_PLATFORM_ANDROID
#error "Android not supported"
#elif defined(__linux__)
#define PK_PLATFORM_LINUX
#define PK_PLATFORM PK_PLATFORM_LINUX
//#error "Linux not supported"
#else
#error "Unknown platform"
#endif
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

//#define BIT(x) (1 << x)

#define PK_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
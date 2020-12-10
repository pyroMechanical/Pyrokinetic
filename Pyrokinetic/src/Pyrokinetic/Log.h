#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Pyrokinetic
{
	class PK_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

	};
}


//core log macros
#define PK_CORE_TRACE(...) ::Pyrokinetic::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PK_CORE_INFO(...)  ::Pyrokinetic::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PK_CORE_WARN(...)  ::Pyrokinetic::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PK_CORE_ERROR(...) ::Pyrokinetic::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PK_CORE_FATAL(...) ::Pyrokinetic::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//core log macros
#define PK_TRACE(...) ::Pyrokinetic::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PK_INFO(...)  ::Pyrokinetic::Log::GetClientLogger()->info(__VA_ARGS__)
#define PK_WARN(...)  ::Pyrokinetic::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PK_ERROR(...) ::Pyrokinetic::Log::GetClientLogger()->error(__VA_ARGS__)
#define PK_FATAL(...) ::Pyrokinetic::Log::GetClientLogger()->fatal(__VA_ARGS__)
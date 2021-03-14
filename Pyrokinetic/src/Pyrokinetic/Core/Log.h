#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace pk
{
	class Log
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
#define PK_CORE_TRACE(...) ::pk::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PK_CORE_INFO(...)  ::pk::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PK_CORE_WARN(...)  ::pk::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PK_CORE_ERROR(...) ::pk::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PK_CORE_FATAL(...) ::pk::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//core log macros
#define PK_TRACE(...) ::pk::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PK_INFO(...)  ::pk::Log::GetClientLogger()->info(__VA_ARGS__)
#define PK_WARN(...)  ::pk::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PK_ERROR(...) ::pk::Log::GetClientLogger()->error(__VA_ARGS__)
#define PK_FATAL(...) ::pk::Log::GetClientLogger()->fatal(__VA_ARGS__)
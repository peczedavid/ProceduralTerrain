#pragma once

#include "Core/Core.h"

#include <spdlog/spdlog.h>

class Log
{
public:
	static void Initialize();

	inline static Ref<spdlog::logger>& GetLogger() { return s_Logger; }
private:
	static Ref<spdlog::logger> s_Logger;
};

#define TRACE(...) Log::GetLogger()->trace(__VA_ARGS__)
#define INFO(...) Log::GetLogger()->info(__VA_ARGS__)
#define WARN(...) Log::GetLogger()->warn(__VA_ARGS__)
#define ERROR(...) Log::GetLogger()->error(__VA_ARGS__)
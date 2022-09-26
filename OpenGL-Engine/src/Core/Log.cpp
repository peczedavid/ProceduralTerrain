#include "pch.h"

#include "Core/Log.h"

Ref<spdlog::logger> Log::s_Logger;

void Log::Initialize()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_Logger = spdlog::stdout_color_mt("APP");
	s_Logger->set_level(spdlog::level::trace);
}

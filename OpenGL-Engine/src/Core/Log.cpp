#include "pch.h"

#include "Core/Log.h"

Ref<Log::ImGuiLog> Log::s_Logger;

void Log::Initialize()
{
	s_Logger = CreateRef<ImGuiLog>();
}

void Log::Draw()
{
	s_Logger->Draw("Log");
}

#include "pch.h"

#include "Core/Application.h"

Application* CreateApplication();

constexpr int gpu = 1;

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = static_cast<DWORD>(gpu);
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = gpu;
}

int WINAPI
WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	auto app = CreateApplication();
	app->Run();
	delete app;
	return 0;
}



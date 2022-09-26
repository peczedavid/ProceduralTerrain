#include "pch.h"

#include "Core/Application.h"

Application* CreateApplication();

constexpr int gpu = 1;

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = static_cast<DWORD>(gpu);
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = gpu;
}

int main(int argc, char** argv)
{
	auto app = CreateApplication();
	app->Run();
	delete app;
	return 0;
}
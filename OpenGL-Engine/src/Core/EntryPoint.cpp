#include "Core/Application.h"

Application* CreateApplication();

int main(int argc, char** argv)
{
	auto app = CreateApplication();
	app->Run();
	return 0;
}
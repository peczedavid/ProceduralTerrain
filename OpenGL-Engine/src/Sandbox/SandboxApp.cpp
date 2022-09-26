#include "pch.h"

#include "Core/Application.h"
#include "Sandbox/GameLayer.h"

class Sandbox : public Application
{
public:
	Sandbox(const WindowProps& props)
		: Application(props)
	{
		TRACE("Pushing GameLayer");
		this->PushLayer(new GameLayer());
	}

	~Sandbox()
	{

	}
};

Application* CreateApplication()
{
	WindowProps windowProps;
	windowProps.Width = 1280;
	windowProps.Height = 720;
	windowProps.Title = "OpenGL Engine";
	windowProps.Maximized = true;

	return new Sandbox(windowProps);
}
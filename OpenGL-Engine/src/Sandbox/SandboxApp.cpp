#include "pch.h"

#include "Core/Application.h"
#include "Sandbox/GameLayer.h"
#include "Sandbox/ClothSimulation/ClothSimulationLayer.h"

class Sandbox : public Application
{
public:
	Sandbox(const WindowProps& props)
		: Application(props)
	{
		/*TRACE("Pushing GameLayer");
		this->PushLayer(new GameLayer());*/
		TRACE("Pushing ClothSimulationLayer");
		this->PushLayer(new ClothSimulationLayer());
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
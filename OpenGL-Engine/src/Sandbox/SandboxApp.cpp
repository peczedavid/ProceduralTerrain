#include "Core/Application.h"
#include "Sandbox/GameLayer.h"

class Sandbox : public Application
{
public:
	Sandbox()
	{
		this->PushLayer(new GameLayer());
	}

	~Sandbox()
	{

	}
};

Application* CreateApplication()
{
	return new Sandbox();
}
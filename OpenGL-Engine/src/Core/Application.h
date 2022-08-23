#pragma once
#include "Core/Window.h"
#include "Core/Layers/LayerStack.h"

class Application
{
public:
	Application();
	virtual ~Application();

	void Run();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	static inline Application& Get() { return *s_Instance; }
	inline Window* GetWindow() { return m_Window; }

private:
	Window* m_Window;
	static Application* s_Instance;
	LayerStack m_LayerStack;
};

Application* CreateApplication();

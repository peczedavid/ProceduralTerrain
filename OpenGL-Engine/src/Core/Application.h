#pragma once
#include "Core/Window.h"

class Application
{
public:
	Application();
	virtual ~Application();

	void Run();

	static inline Application& Get() { return *s_Instance; }
	inline Window GetWindow() { return *m_Window; }

private:
	Window* m_Window;
	static Application* s_Instance;
};

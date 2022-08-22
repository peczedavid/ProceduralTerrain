#include "Core/Application.h"

Application* Application::s_Instance = nullptr;

Application::Application()
{
	m_Window = new Window();
	s_Instance = this;
}

Application::~Application()
{
}

void Application::Run()
{
	while (!m_Window->ShouldClose())
	{
		glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_Window->OnUpdate();
	}
}

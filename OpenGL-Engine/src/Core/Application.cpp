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
		static float lastFrameTime = 0.0f;
		float time = (float)glfwGetTime();
		float dt = time - lastFrameTime;
		lastFrameTime = time;

		// Update the layers from bottom to top
		for (Layer* layer : m_LayerStack)
			layer->OnUpdate(dt);

		m_Window->OnUpdate();
	}
}

void Application::PushLayer(Layer* layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay)
{
	m_LayerStack.PushOverlay(overlay);
	overlay->OnAttach();
}

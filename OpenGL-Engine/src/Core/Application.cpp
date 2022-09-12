#include "Core/Application.h"
#include "Rendering/Renderer.h"

Application* Application::s_Instance = nullptr;

Application::Application(const WindowProps& props)
	: m_Cursor(false), m_ImGui(true)
{
	m_Window = new Window(props);
	m_Window->SetCursor(m_Cursor);
	s_Instance = this;

	Renderer::SetOpenGLConfig();
	
	m_LayerStack = new LayerStack();

	m_ImGuiLayer = new ImGuiLayer();
	this->PushOverlay(m_ImGuiLayer);
}

Application::~Application()
{
	delete m_LayerStack;
	delete m_Window;
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
		for (Layer* layer : *m_LayerStack)
			layer->OnUpdate(dt);

		if (m_ImGui)
		{
			m_ImGuiLayer->Begin();
			for (Layer* layer : *m_LayerStack)
				layer->OnImGuiRender(dt);
			m_ImGuiLayer->End();
		}

		m_Window->OnUpdate();
	}
}

void Application::PushLayer(Layer* layer)
{
	m_LayerStack->PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay)
{
	m_LayerStack->PushOverlay(overlay);
	overlay->OnAttach();
}

void Application::ToggleImGui()
{
	m_ImGui = !m_ImGui;
}

void Application::OnResize(uint32_t width, uint32_t height)
{
	m_Window->SetWidth(width);
	m_Window->SetWidth(height);

	for (Layer* layer : *m_LayerStack)
		layer->OnResize(width, height);
}

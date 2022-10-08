#include "pch.h"

#include "Core/Application.h"
#include "Rendering/Renderer.h"

Application* Application::s_Instance = nullptr;

Application::Application(const WindowProps& props)
	: m_Cursor(false)
{
	s_Instance = this;
	Log::Initialize();
	srand(static_cast<uint32_t>(time(0)));

	m_Window = new Window(props);
	TRACE("Created window");
	m_Window->SetCursor(m_Cursor);

	Renderer::Initialize();
	TRACE("Initialized renderer");

	m_LayerStack = new LayerStack();
	TRACE("Created LayerStack");

	m_ImGuiLayer = new ImGuiLayer();
	this->PushOverlay(m_ImGuiLayer);
	TRACE("Created ImGuiLayer");
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
		const float time = (float)glfwGetTime();
		const float dt = time - lastFrameTime;
		lastFrameTime = time;

		// Update the layers from bottom to top
		for (Layer* layer : *m_LayerStack)
			layer->OnUpdate(dt);

		m_ImGuiLayer->Begin();
		for (Layer* layer : *m_LayerStack)
			layer->OnImGuiRender(dt);
		m_ImGuiLayer->End();

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

void Application::Close()
{
	glfwSetWindowShouldClose(m_Window->GetNativeWindow(), GLFW_TRUE);
}

void Application::OnResize(const uint32_t width, const uint32_t height)
{
	m_Window->SetWidth(width);
	m_Window->SetWidth(height);
}

void Application::OnScreenshot()
{
	for (Layer* layer : *m_LayerStack)
		layer->OnScreenshot();
}

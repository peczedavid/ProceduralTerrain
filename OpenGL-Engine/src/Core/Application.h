#pragma once

#include "Core/Window.h"
#include "Core/Layers/LayerStack.h"
#include "Rendering/ImGui/ImGuiLayer.h"

class Application
{
public:
	Application(const WindowProps& props = WindowProps());
	Application(const Application& application) = delete;
	virtual ~Application();

	void Run();
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	static inline Application& Get() { return *s_Instance; }
	inline Window* GetWindow() { return m_Window; }
public:
	void OnResize(uint32_t width, uint32_t height);
	void OnScreenshot();
private:
	LayerStack* m_LayerStack;
	Window* m_Window;
	bool m_Cursor;
	ImGuiLayer* m_ImGuiLayer;

	static Application* s_Instance;
public:
	inline bool IsCursor() { return m_Cursor; }
	inline const bool IsCursor() const { return m_Cursor; }
	inline void SetCursor(bool cursor) { m_Cursor = cursor; }
};

Application* CreateApplication();
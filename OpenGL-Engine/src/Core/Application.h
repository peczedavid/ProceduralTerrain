#pragma once
#include "Core/Window.h"
#include "Core/Layers/LayerStack.h"
#include "Rendering/ImGui/ImGuiLayer.h"

class Application
{
public:
	Application(const WindowProps& props = WindowProps());
	virtual ~Application();

	void Run();
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	void ToggleImGui();

	static inline Application& Get() { return *s_Instance; }
	inline Window* GetWindow() { return m_Window; }
public:
	void OnResize(uint32_t width, uint32_t height);
private:
	LayerStack* m_LayerStack;
	Window* m_Window;
	bool m_Cursor;
	bool m_ImGui;
	ImGuiLayer* m_ImGuiLayer;

	static Application* s_Instance;
public:
	inline bool IsCursor() { return m_Cursor; }
	inline const bool IsCursor() const { return m_Cursor; }
	inline void SetCursor(bool cursor) { m_Cursor = cursor; }
};

Application* CreateApplication();

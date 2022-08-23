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
	bool m_Cursor;
	static Application* s_Instance;
	LayerStack m_LayerStack;
public:
	inline bool IsCursor() { return m_Cursor; }
	inline const bool IsCursor() const { return m_Cursor; }
	inline void SetCursor(bool cursor) { m_Cursor = cursor; }
};

Application* CreateApplication();

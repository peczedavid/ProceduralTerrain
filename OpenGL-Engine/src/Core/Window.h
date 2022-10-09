#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

struct WindowProps
{
	std::string Title;
	unsigned int Width;
	unsigned int Height;
	bool Maximized;

	WindowProps(const std::string& title = "GLFW window",
		unsigned int width = 1280,
		unsigned int height = 720,
		bool maximized = false)
		: Title(title), Width(width), Height(height), Maximized(maximized)
	{
	}
};

class Window
{
public:
	Window(const WindowProps& props);
	~Window();

	void OnUpdate();

	bool ShouldClose() { return glfwWindowShouldClose(m_Window); }

	void SetCursor(bool cursor) const { glfwSetInputMode(m_Window, GLFW_CURSOR, cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); }

	void ToggleMaximized();
	void Maximize();
	void Restore();

	GLFWwindow* GetNativeWindow() const { return m_Window; }
private:
	uint32_t m_Height, m_Width;
	GLFWwindow* m_Window;
	bool m_VSync;
	bool m_Maximized;

public:
	uint32_t GetWidth() { return m_Width; }
	const uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() { return m_Height; }
	const uint32_t GetHeight() const { return m_Height; }
	void SetHeight(uint32_t height) { m_Height = height; }
	void SetWidth(uint32_t width) { m_Width = width; }

	
	void SetVSync(bool enabled) { m_VSync = enabled; glfwSwapInterval(m_VSync ? 1 : 0); }
	bool IsVSync() const { return m_VSync; }
};

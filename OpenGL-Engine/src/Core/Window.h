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

	WindowProps(const std::string& title = "OpenGL/GLFW window",
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
	virtual ~Window();

	virtual void OnUpdate();

	inline bool ShouldClose() { return glfwWindowShouldClose(m_Window); }

	inline void SetCursor(bool cursor) const { glfwSetInputMode(m_Window, GLFW_CURSOR, cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); }

	virtual GLFWwindow* GetNativeWindow() const { return m_Window; }
private:
	uint32_t m_Height, m_Width;
	GLFWwindow* m_Window;
	bool m_VSync;

public:
	inline uint32_t GetWidth() { return m_Width; }
	inline const uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() { return m_Height; }
	inline const uint32_t GetHeight() const { return m_Height; }
	inline void SetHeight(uint32_t height) { m_Height = height; }
	inline void SetWidth(uint32_t width) { m_Width = width; }

	
	inline void SetVSync(bool enabled) { m_VSync = enabled; glfwSwapInterval(m_VSync ? 1 : 0); }
	inline bool IsVSync() const { return m_VSync; }
};

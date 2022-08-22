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

	WindowProps(const std::string& title = "OpenGL/GLFW window",
		unsigned int width = 1280,
		unsigned int height = 720)
		: Title(title), Width(width), Height(height)
	{
	}
};

class Window
{
public:
	Window(const WindowProps& props = WindowProps());
	virtual ~Window();

	virtual void OnUpdate();

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }

	inline void SetVSync(bool enabled) { m_VSync = enabled; }
	inline bool IsVSync() const { return m_VSync; }

	inline bool ShouldClose() { return glfwWindowShouldClose(m_Window); }

	virtual GLFWwindow* GetNativeWindow() const { return m_Window; }
private:
	uint32_t m_Height, m_Width;
	GLFWwindow* m_Window;
	bool m_VSync;
};

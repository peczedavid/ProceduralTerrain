#include "pch.h"

#include "Core/Core.h"
#include "Core/Window.h"
#include "Core/Application.h"
#include "Rendering/Renderer.h"

void error_callback(int error, const char* description)
{
	ERROR("{0}", description);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto& app = Application::Get();

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		app.SetCursor(!Application::Get().IsCursor());
		glfwSetInputMode(window, GLFW_CURSOR, app.IsCursor() ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		Renderer::TogglePolygonMode();
	}

	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
	{
		app.OnScreenshot();
	}

	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
	{
		Renderer::DebugView = !Renderer::DebugView;
	}
}

Window::Window(const WindowProps& props)
	: m_Width(props.Width), m_Height(props.Height)
{
	ASSERT(glfwInit(), "GLFW Initialization failed!");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	if(props.Maximized)
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

	m_Window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), NULL, NULL);
	ASSERT(m_Window, "Window creation failed!");

	if (!props.Maximized)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		const int x = (mode->width - props.Width) / 2;
		const int y = (mode->height - props.Height) / 2;
		glfwSetWindowPos(m_Window, x, y);
	}

	if (props.Maximized)
	{
		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		m_Width = width, m_Height = height;
	}

	glfwMakeContextCurrent(m_Window);
	ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "OpenGL context creation failed!");

	m_VSync = true;
	glfwSwapInterval(m_VSync ? 1 : 0);

	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(m_Window, key_callback);
	glfwSetWindowSizeCallback(m_Window, window_size_callback);

	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

Window::~Window()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Window::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
}

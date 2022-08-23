#include "Core/Window.h"
#include "Core/Application.h"
#include "Rendering/Renderer.h"

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Application::Get().GetWindow()->SetWidth(width);
	Application::Get().GetWindow()->SetHeight(height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		Application::Get().SetCursor(!Application::Get().IsCursor());
		glfwSetInputMode(window, GLFW_CURSOR, Application::Get().IsCursor() ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		Renderer::TogglePolygonMode();
	}
}

Window::Window(const WindowProps& props)
	: m_Width(props.Width), m_Height(props.Height)
{
	if (!glfwInit())
		printf("Initialization failed!");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	m_Window = glfwCreateWindow(props.Width, props.Height, props.Title.c_str(), NULL, NULL);
	if (!m_Window)
		printf("Window or OpenGL context creation failed!");
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	const int x = (mode->width - props.Width) / 2;
	const int y = (mode->height - props.Height) / 2;
	glfwSetWindowPos(m_Window, x, y);

	m_VSync = false;

	glfwMakeContextCurrent(m_Window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

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

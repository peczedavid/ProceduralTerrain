#include "Core/Window.h"
#include "Core/Application.h"

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		static bool cursor = true;
		cursor = !cursor;
		glfwSetInputMode(window, GLFW_CURSOR, cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		// Renderer -> Wireframe toggle
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

	m_VSync = false;

	glfwMakeContextCurrent(m_Window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(m_Window, key_callback);
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

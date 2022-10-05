#include "pch.h"

#include "Rendering/TrackballCamera.h"
#include "Core/Application.h"

TrackballCamera::TrackballCamera(const float radius, const glm::vec3& lookat)
	: m_Radius(radius), m_LookAt(lookat), m_Up({0.0f, 1.0f, 0.0f})
{
}

// 0 <= theta <= pi (0 = top most position, pi = lower most position)
// 0 <= phi <= 2pi (0 = looking from +x towars -x)
// 
// x = r * sin(theta) * cos(phi)
// y = r * cos(theta)
// z = r * sin(theta) * sin(phi)

void TrackballCamera::UpdateMatrix(const float fovDeg, const float nearPlane, const float farPlane)
{
	m_Proj = glm::mat4(1.0f);
	m_View = glm::mat4(1.0f);

	float x = m_Radius * sinf(m_Theta) * cosf(m_Phi);
	float y = m_Radius * cosf(m_Theta);
	float z = m_Radius * sinf(m_Theta) * sinf(m_Phi);

	m_Position = glm::vec3(x, y, z);

	m_View = glm::lookAt(glm::vec3(x, y, z), glm::vec3(0, 0, 0), m_Up);
	m_Proj = glm::perspective(
		glm::radians(fovDeg),
		(float)m_Width / (float)m_Height,
		nearPlane,
		farPlane
	);
	m_ViewProj = m_Proj * m_View;
}

void TrackballCamera::Update(const float dt)
{
	static bool first = true;
	GLFWwindow* glfwWindow = Application::Get().GetWindow()->GetNativeWindow();

	if (glfwGetKey(glfwWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		Zoom(25.0f * dt);
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		Zoom(-25.0f * dt);
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		m_Phi += 3.141f / 3.0f * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		m_Phi -= 3.141f / 3.0f * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_UP) == GLFW_PRESS)
	{
		m_Theta -= 3.141f / 3.0f * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		m_Theta += 3.141f / 3.0f * dt;
	}
	m_Radius = glm::clamp(m_Radius, 1.0f, FLT_MAX);
}

void TrackballCamera::Zoom(float dist)
{
	m_Radius -= dist;
}

void TrackballCamera::Resize(const uint32_t width, const uint32_t height)
{
	m_Width = width;
	m_Height = height;
}
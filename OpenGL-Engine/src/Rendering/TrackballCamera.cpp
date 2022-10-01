#include "pch.h"

#include "Rendering/TrackballCamera.h"
#include "Core/Application.h"

TrackballCamera::TrackballCamera(const float radius, const glm::vec3& lookat)
	: m_Radius(radius), m_LookAt(lookat), m_Up({0.0f, 1.0f, 0.0f})
{
}

void TrackballCamera::UpdateMatrix(const float fovDeg, const float nearPlane, const float farPlane)
{
	m_Proj = glm::mat4(1.0f);
	m_View = glm::mat4(1.0f);

	m_Position = glm::vec3(0, 1, 1) * m_Radius;

	m_View = glm::lookAt(m_Position, m_LookAt, m_Up);
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

	if (glfwGetKey(glfwWindow, GLFW_KEY_UP) == GLFW_PRESS)
	{
		m_Radius -= 25.0f * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		m_Radius += 25.0f * dt;
	}

	m_Radius = glm::clamp(m_Radius, 1.0f, FLT_MAX);
}

void TrackballCamera::Resize(const uint32_t width, const uint32_t height)
{
	m_Width = width;
	m_Height = height;
}
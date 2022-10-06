#include "pch.h"

#include "Rendering/Camera.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <Core/Application.h>

FPSCamera::FPSCamera(const glm::vec3& position, const glm::vec3& orientation)
	: m_Position(position), m_Orientation(glm::normalize(orientation)), m_Up({ 0.0f, 1.0f, 0.0f })
{
}

void FPSCamera::CalculateMatrix(const float fovDeg, const float nearPlane, const float farPlane)
{
	m_Proj = glm::mat4(1.0f);
	m_View = glm::mat4(1.0f);

	m_View = glm::lookAt(m_Position, m_Position + m_Orientation, m_Up);
	m_Proj = glm::perspective(
		glm::radians(fovDeg),
		(float)m_Width / (float)m_Height,
		nearPlane,
		farPlane
	);
	m_CameraMatrix = m_Proj * m_View;
}

void FPSCamera::Update(const float dt)
{
	if (Application::Get().IsCursor())
		return;

	GLFWwindow* glfwWindow = Application::Get().GetWindow()->GetNativeWindow();

	if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_Position += m_Speed * m_Orientation * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_Position -= m_Speed * m_Orientation * dt;
	}

	if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		glm::vec3 dir = glm::normalize(glm::cross(m_Up, m_Orientation));
		m_Position += m_Speed * dir * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		glm::vec3 dir = glm::normalize(glm::cross(m_Up, m_Orientation));
		m_Position -= m_Speed * dir * dt;
	}

	if (glfwGetKey(glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		glm::vec3 dir = glm::normalize(m_Up);
		m_Position += m_Speed * dir * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_C) == GLFW_PRESS)
	{
		glm::vec3 dir = glm::normalize(m_Up);
		m_Position -= m_Speed * dir * dt;
	}

	if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		m_SpeedMultiplier = 10.f;
	}

	if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		m_SpeedMultiplier = 0.15f;
	}

	if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE
		&& glfwGetKey(glfwWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
	{
		m_SpeedMultiplier = 1.f;
	}

	m_Speed = m_BaseSpeed * m_SpeedMultiplier;

	static bool firstLook = true;
	if (firstLook)
	{
		glfwSetCursorPos(glfwWindow, (m_Width / 2), (m_Height / 2));
		firstLook = false;
	}

	double mouseX, mouseY;
	glfwGetCursorPos(glfwWindow, &mouseX, &mouseY);

	const float rotX = m_Sensitivity * (float)(mouseY - (m_Height / 2)) / m_Height;
	const float rotY = m_Sensitivity * (float)(mouseX - (m_Width / 2)) / m_Width;

	glm::vec3 newOrientation = glm::rotate(m_Orientation, glm::radians(-rotX), glm::normalize(glm::cross(m_Orientation, m_Up)));
	if (!(glm::angle(newOrientation, m_Up) <= glm::radians(5.f) || glm::angle(newOrientation, -m_Up) < glm::radians(5.f)))
	{
		m_Orientation = newOrientation;
	}

	m_Orientation = glm::rotate(m_Orientation, glm::radians(-rotY), m_Up);

	glfwSetCursorPos(glfwWindow, (m_Width / 2), (m_Height / 2));
}

void FPSCamera::Resize(const uint32_t width, const uint32_t height)
{
	m_Width = width;
	m_Height = height;
}


TrackballCamera::TrackballCamera(const float radius, const glm::vec3& lookat)
	: m_Radius(radius), m_LookAt(lookat), m_Up({ 0.0f, 1.0f, 0.0f })
{
	m_Position = GetCartesian();
}

// 0 <= theta <= pi (0 = top most position, pi = lower most position)
// 0 <= phi <= 2pi (0 = looking from +x towars -x)
// 
// x = r * sin(theta) * cos(phi)
// y = r * cos(theta)
// z = r * sin(theta) * sin(phi)

void TrackballCamera::CalculateMatrix(const float fovDeg, const float nearPlane, const float farPlane)
{
	m_Proj = glm::mat4(1.0f);
	m_View = glm::mat4(1.0f);

	m_Position = GetCartesian() + m_LookAt;

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
	if (!Application::Get().IsCursor())
		return;

	GLFWwindow* glfwWindow = Application::Get().GetWindow()->GetNativeWindow();

	if (glfwGetKey(glfwWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		m_Radius -= 200.0f * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		m_Radius -= -200.0f * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		m_Phi += 3.141f * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		m_Phi -= 3.141f * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_UP) == GLFW_PRESS)
	{
		m_Theta -= 3.141f * dt;
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		m_Theta += 3.141f * dt;
	}
	glm::vec3 look = glm::normalize(GetCartesian());
	glm::vec3 right = glm::cross(look, m_Up);
	glm::vec3 up = glm::cross(look, right);
	if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_LookAt = m_LookAt + (right * -100.0f * dt);
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_LookAt = m_LookAt + (right * 100.0f * dt);
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_LookAt = m_LookAt + (up * -100.0f * dt);
	}
	if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_LookAt = m_LookAt + (up * 100.0f * dt);
	}

	m_Radius = glm::clamp(m_Radius, 1.0f, FLT_MAX);
	m_Theta = glm::clamp(m_Theta, 0.141f, 3.0f);
}

void TrackballCamera::Resize(const uint32_t width, const uint32_t height)
{
	m_Width = width;
	m_Height = height;
}

glm::vec3 TrackballCamera::GetCartesian()
{
	float x = m_Radius * sinf(m_Theta) * cosf(m_Phi);
	float y = m_Radius * cosf(m_Theta);
	float z = m_Radius * sinf(m_Theta) * sinf(m_Phi);

	return glm::vec3(x, y, z);
}

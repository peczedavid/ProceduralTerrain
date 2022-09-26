#include "pch.h"

#include "Rendering/Camera.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <Core/Application.h>

Camera::Camera(const glm::vec3& position, const glm::vec3& orientation)
	: m_Position(position), m_Orientation(glm::normalize(orientation)), m_Up({ 0.0f, 1.0f, 0.0f })
{
}

void Camera::UpdateMatrix(const float fovDeg, const float nearPlane, const float farPlane)
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

void Camera::Update(const float dt)
{
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

void Camera::Resize(const uint32_t width, const uint32_t height)
{
	m_Width = width;
	m_Height = height;
}

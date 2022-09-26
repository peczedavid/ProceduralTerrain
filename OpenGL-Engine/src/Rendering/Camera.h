#pragma once

#include <glm/glm.hpp>

class Camera {
public:
	Camera(const glm::vec3& position, const glm::vec3& orientation = { 0.0f, 0.0f, -1.0f});

	void UpdateMatrix(const float fovDeg, const float nearPlane, const float farPlane);
	void Update(const float dt);

	void Resize(const uint32_t width, const uint32_t height);
private:
	glm::vec3 m_Position;
	glm::vec3 m_Orientation;
	glm::vec3 m_Up;
	
	glm::mat4 m_View;
	glm::mat4 m_Proj;
	glm::mat4 m_CameraMatrix;

	uint32_t m_Width, m_Height;

	float m_BaseSpeed = 40.f;
	float m_SpeedMultiplier = 1.f;
	float m_Speed = m_BaseSpeed * m_SpeedMultiplier;
	float m_Sensitivity = 50.f;
public:
	inline glm::vec3& GetRight() { return glm::cross(glm::normalize(m_Orientation), glm::normalize(m_Up)); }
	inline const glm::vec3& GetRight() const { return glm::cross(glm::normalize(m_Orientation), glm::normalize(m_Up)); }

	inline glm::mat4& GetMatrix() { return m_CameraMatrix; }
	inline const glm::mat4& GetMatrix() const { return m_CameraMatrix; }

	inline glm::vec3& GetPosition() { return m_Position; }
	inline const glm::vec3& GetPosition() const { return m_Position; }
	inline void SetPosition(const glm::vec3& position) { m_Position = position; }

	inline glm::vec3& GetOrientation() { return m_Orientation; }
	inline const glm::vec3& GetOrientation() const { return m_Orientation; }

	inline glm::vec3& GetLookAt() { return m_Position + m_Orientation; }
	inline const glm::vec3& GetLookAt() const { return m_Position + m_Orientation; }

	inline glm::vec3& GetUp() { return m_Up; }
	inline const glm::vec3& GetUp() const { return m_Up; }

	inline glm::mat4& GetView() { return m_View; }
	inline const glm::mat4& GetView() const { return m_View; }
	inline glm::mat4& GetProj() { return m_Proj; }
	inline const glm::mat4& GetProj() const { return m_Proj; }
};
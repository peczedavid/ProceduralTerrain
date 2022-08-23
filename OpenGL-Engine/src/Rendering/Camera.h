#pragma once

#include <glm/glm.hpp>

class Camera {
public:
	

	Camera(glm::vec3 position);

	void UpdateMatrix(float fovDeg, float nearPlane, float farPlane);
	void Update(float dt);
private:
	glm::vec3 m_Position;
	glm::vec3 m_Orientation;
	glm::vec3 m_Up;
	// Make static in function
	bool m_FirstLook = true;
	
	glm::mat4 m_View;
	glm::mat4 m_Proj;
	glm::mat4 m_CameraMatrix;

	float m_BaseSpeed = 3.f;
	float m_SpeedMultiplier = 1.f;
	float m_Speed = m_BaseSpeed * m_SpeedMultiplier;
	float m_Sensitivity = 50.f;
public:
	inline glm::mat4& GetMatrix() { return m_CameraMatrix; }
	inline const glm::mat4& GetMatrix() const { return m_CameraMatrix; }

	inline glm::vec3& GetPosition() { return m_Position; }
	inline const glm::vec3& GetPosition() const { return m_Position; }
	inline void SetPosition(const glm::vec3& position) { m_Position = position; }
};
#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	virtual void CalculateMatrix(const float fovDef, const float nearPlane, const float farPlane) = 0;
	virtual void Update(const float dt) = 0;

	virtual void Resize(const uint32_t width, const uint32_t height) = 0;

	virtual void SetPosition(const glm::vec3& position) = 0;

	virtual glm::vec3& GetPosition() = 0;
	virtual glm::vec3& GetOrientation() = 0;
	virtual glm::vec3& GetLookAt() = 0;
	virtual glm::vec3& GetUp() = 0;
	virtual glm::mat4& GetView() = 0;
	virtual glm::mat4& GetProj() = 0;
	virtual glm::mat4& GetViewProj() = 0;
};

class FPSCamera : public Camera {
public:
	FPSCamera(const glm::vec3& position, const glm::vec3& orientation = { 0.0f, 0.0f, -1.0f});

	void CalculateMatrix(const float fovDeg, const float nearPlane, const float farPlane) override;
	void Update(const float dt) override;

	void Resize(const uint32_t width, const uint32_t height) override;
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

	void SetPosition(const glm::vec3& position) override { m_Position = position; }

	glm::vec3& GetPosition() override { return m_Position; }
	glm::vec3& GetOrientation() override { return m_Orientation; }
	glm::vec3& GetLookAt() override { return m_Position + m_Orientation; }
	glm::vec3& GetUp() override { return m_Up; }
	glm::mat4& GetView() override { return m_View; }
	glm::mat4& GetProj() override { return m_Proj; }
	glm::mat4& GetViewProj() override { return m_CameraMatrix; }
};

class TrackballCamera : public Camera
{
public:
	TrackballCamera(const float radius, const glm::vec3& lookat);
	TrackballCamera(const TrackballCamera&) = delete;
	TrackballCamera operator=(const TrackballCamera&) = delete;
	~TrackballCamera() = default;

	void CalculateMatrix(const float fovDeg, const float nearPlane, const float farPlane) override;
	void Update(const float dt) override;

	void Resize(const uint32_t width, const uint32_t height) override;
private:
	glm::mat4 m_View, m_Proj, m_ViewProj;
	glm::vec3 m_Position;
	glm::vec3 m_LookAt;
	glm::vec3 m_Up;
	float m_Theta = 3.141f / 4.0f;
	float m_Phi = 3.141f / 2.0f;
	float m_Radius;
	uint32_t m_Width, m_Height;
public:
	float GetRadius() { return m_Radius; }
	glm::vec3 GetCartesian();

	// TODO: Recalculate m_Phi, m_Theta
	void SetPosition(const glm::vec3& position) override { m_Position = position; }

	glm::vec3& GetPosition() override { return m_Position; }
	glm::vec3& GetOrientation() override { return glm::normalize(m_Position - m_LookAt); }
	glm::vec3& GetLookAt() override { return m_LookAt; }
	glm::vec3& GetUp() override { return m_Up; }
	glm::mat4& GetView() override { return m_View; }
	glm::mat4& GetProj() override { return m_Proj; }
	glm::mat4& GetViewProj() override { return m_ViewProj; }
};
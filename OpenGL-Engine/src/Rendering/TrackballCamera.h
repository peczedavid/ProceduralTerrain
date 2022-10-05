#pragma once

#include <glm/glm.hpp>

class TrackballCamera
{
public:
	TrackballCamera(const float radius, const glm::vec3& lookat);
	TrackballCamera(const TrackballCamera&) = delete;
	TrackballCamera operator=(const TrackballCamera&) = delete;
	~TrackballCamera() = default;

	void UpdateMatrix(const float fovDeg, const float nearPlane, const float farPlane);
	void Update(const float dt);

	void Zoom(float dist);
	void Resize(const uint32_t width, const uint32_t height);
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
	inline float GetRadius() { return m_Radius; }
	inline const float GetRadius() const { return m_Radius; }
	inline glm::mat4& GetViewProj() { return m_ViewProj; }
	inline const glm::mat4& GetViewProj() const { return m_ViewProj; }
	inline glm::mat4& GetView() { return m_View; }
	inline const glm::mat4& GetView() const { return m_View; }
	inline glm::mat4& GetProj() { return m_Proj; }
	inline const glm::mat4& GetProj() const { return m_Proj; }
	inline glm::vec3& GetPosition() { return m_Position; }
	inline const glm::vec3& GetPosition() const { return m_Position; }
	inline void SetPosition(const glm::vec3& position) { m_Position = position; }
	inline glm::vec3& GetLookAt() { return m_LookAt; }
	inline const glm::vec3& GetLookAt() const { return m_LookAt; }
	inline glm::vec3& GetUp() { return m_Up; }
	inline const glm::vec3& GetUp() const { return m_Up; }
};
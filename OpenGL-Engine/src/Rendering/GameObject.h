#pragma once

#include "Rendering/Geometry/Model.h"

class GameObject
{
public:
	GameObject(Model* m_Model);
	GameObject(const GameObject&) = delete;
	GameObject operator=(const GameObject&) = delete;
	~GameObject();

	void Draw(const Shader& shader);
	
	void SetPosition(const glm::vec3& position);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const float scale);
	void SetScale(const glm::vec3& scale);

	glm::vec3& GetPosition();
	glm::vec3& GetRotation();
	glm::vec3& GetScale();
private:
	void CalculateTransform();
private:
	Model* m_Model;
	glm::mat4 m_Transform;
	glm::vec3 m_Translation, m_EulerRotation, m_Scale;
};
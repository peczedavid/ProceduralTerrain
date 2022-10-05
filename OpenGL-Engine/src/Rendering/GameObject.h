#pragma once

#include "Rendering/Geometry/Model.h"
#include "Rendering/Material.h"
#include "Core/Core.h"

class GameObject
{
public:
	GameObject(Model* model, Ref<Material> material);
	GameObject(const GameObject&) = delete;
	GameObject operator=(const GameObject&) = delete;
	~GameObject();

	void Draw();
	
	void Set(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	void SetPosition(const glm::vec3& position);
	void SetRotation(const glm::vec3& rotation);
	void SetScale(const float scale);
	void SetScale(const glm::vec3& scale);

	glm::mat4& GetTransform();

	glm::vec3& GetPosition();
	glm::vec3& GetRotation();
	glm::vec3& GetScale();
	Ref<Material> GetMaterial();
private:
	void CalculateTransform();
private:
	Model* m_Model;
	Ref<Material> m_Material;
	glm::mat4 m_Transform;
	glm::vec3 m_Translation, m_EulerRotation, m_Scale;
};
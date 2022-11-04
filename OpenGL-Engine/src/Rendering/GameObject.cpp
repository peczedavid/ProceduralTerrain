#include "pch.h"

#include "Rendering/GameObject.h"

GameObject::GameObject(Model* model, Ref<Material> material)
	: m_Model(model),
	  m_Material(material),
	  m_Translation(0.0f, 0.0, 0.0f),
	  m_EulerRotation(0.0f, 0.0f, 0.0f),
	  m_Scale(1.0f, 1.0f, 1.0f),
	  m_Transform(glm::mat4(1.0f))
{

}

GameObject::~GameObject()
{
}

void GameObject::Draw()
{
	if (m_Model == nullptr) return;
	m_Material->SetUniforms();
	m_Material->GetShader()->SetUniform("u_Model", m_Transform);
	m_Model->Draw();
}

void GameObject::Set(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
{
	m_Translation = position;
	m_EulerRotation = rotation;
	m_Scale = scale;
}

void GameObject::SetPosition(const glm::vec3& position)
{
	m_Translation = position;
	CalculateTransform();
}

void GameObject::SetRotation(const glm::vec3& rotation)
{
	m_EulerRotation = rotation;
	CalculateTransform();
}

void GameObject::SetScale(const float scale)
{
	m_Scale = glm::vec3(scale, scale, scale);
	CalculateTransform();
}

void GameObject::SetScale(const glm::vec3& scale)
{
	m_Scale = scale;
	CalculateTransform();
}

//void GameObject::SetTransform(const glm::mat4& transform)
//{
//	m_Transform = transform;
//}

glm::mat4& GameObject::GetTransform()
{
	return m_Transform;
}

glm::vec3& GameObject::GetPosition()
{
	return m_Translation;
}

glm::vec3& GameObject::GetRotation()
{
	return m_EulerRotation;
}

glm::vec3& GameObject::GetScale()
{
	return m_Scale;
}

Ref<Material> GameObject::GetMaterial()
{
	return m_Material;
}

void GameObject::CalculateTransform()
{
	m_Transform = glm::translate(glm::identity<glm::mat4>(), m_Translation);
	m_Transform = m_Transform * glm::eulerAngleYXZ(m_EulerRotation.y, m_EulerRotation.x, m_EulerRotation.z);
	m_Transform = glm::scale(m_Transform, m_Scale);
}

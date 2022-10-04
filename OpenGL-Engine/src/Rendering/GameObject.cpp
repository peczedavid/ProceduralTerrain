#include "pch.h"

#include "Rendering/GameObject.h"

GameObject::GameObject(Model* model)
	: m_Model(model),
	  m_Translation(0.0f, 0.0, 0.0f),
	  m_EulerRotation(0.0f, 0.0f, 0.0f),
	  m_Scale(1.0f, 1.0f, 1.0f),
	  m_Transform(glm::mat4(1.0f))
{

}

GameObject::~GameObject()
{
}

void GameObject::Draw(const Shader& shader)
{
	shader.SetUniform("u_Model", m_Transform);
	m_Model->Draw();
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

void GameObject::CalculateTransform()
{
	glm::mat4 model = glm::identity<glm::mat4>();
	model = glm::translate(model, m_Translation);
	model = model * glm::eulerAngleYXZ(m_EulerRotation.y, m_EulerRotation.x, m_EulerRotation.z);
	model = glm::scale(model, m_Scale);
	m_Transform = model;
}

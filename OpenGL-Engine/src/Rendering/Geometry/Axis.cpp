#include "pch.h"

#include "Rendering/Geometry/Axis.h"

Axis::Axis(Ref<Shader> shader)
	: m_Model(glm::mat4(1.0f)), m_Shader(shader)
{
	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);

	glBindVertexArray(m_Vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	constexpr float vertices[] = {
		// X - Red
		0.0f, 0.0f, 0.0f, 0.9f, 0.2f, 0.15f,
		1.0f, 0.0f, 0.0f, 0.9f, 0.2f, 0.15f,
		// Y - Green
		0.0f, 0.0f, 0.0f, 0.15f, 0.9f, 0.2f,
		0.0f, 1.0f, 0.0f, 0.15f, 0.9f, 0.2f,
		// Z - Blue
		0.0f, 0.0f, 0.0f, 0.15f, 0.2f, 0.9f,
		0.0f, 0.0f, -1.0f, 0.15f, 0.2f, 0.9f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Axis::~Axis()
{
}

void Axis::Render(Ref<Camera> camera)
{
	m_Shader->Use();
	const glm::vec3 orientation = camera->GetOrientation();
	constexpr float pi2 = glm::pi<float>() / 2.0f;
	m_Model = glm::mat4(1.0f);
	m_Model = glm::scale(m_Model, glm::vec3(1.0f, 16.0f / 10.0f, 10.0f / 16.0f) * 0.05f);
	m_Model = glm::rotate(m_Model, orientation.y * pi2, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::vec2 horizontalOrientation = glm::normalize(glm::vec2(orientation.x, orientation.z));
	if (horizontalOrientation.y /*orientation.z*/ < 0.0f)
	{
		m_Model = glm::rotate(m_Model, -horizontalOrientation.x* pi2, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
	{
		m_Model = glm::rotate(m_Model, 2.0f * pi2, glm::vec3(0.0f, 1.0f, 0.0f));
		m_Model = glm::rotate(m_Model, (horizontalOrientation.x) * pi2, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	m_Shader->SetUniform("u_Model", m_Model);
	glBindVertexArray(m_Vao);
	glLineWidth(3.5f);
	glDrawArrays(GL_LINES, 0, 6);
	glLineWidth(1.0f);
}

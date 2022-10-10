#include "pch.h"
#include "Grid.h"

Grid::Grid(const Ref<Shader> shader)
	: m_Shader(shader.get())
{
	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);

	glBindVertexArray(m_Vao);

	glm::vec3 vertices[6] = {
		glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f),
		glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f)
	};

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Grid::Draw()
{
	m_Shader->Use();
	glBindVertexArray(m_Vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

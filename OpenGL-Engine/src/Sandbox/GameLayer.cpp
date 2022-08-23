#include "Sandbox/GameLayer.h"
#include <glad/glad.h>

GameLayer::GameLayer()
{
	m_Shader = new Shader("src/Rendering/Shaders/default.vert", "src/Rendering/Shaders/default.frag");

	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);

	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	float vertices[] = {
	-0.5f, -0.5f,
	 0.5f, -0.5f,
	 0.0f,  0.5f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
}

void GameLayer::OnUpdate(float dt)
{
	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_Shader->Use();
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

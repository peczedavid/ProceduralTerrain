#include "Sandbox/GameLayer.h"
#include <glad/glad.h>
#include "Rendering/Texture2D.h"

GameLayer::GameLayer()
{
	m_Shader = new Shader("src/Rendering/Shaders/default.vert", "src/Rendering/Shaders/default.frag");

	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ebo);

	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	float vertices[] = {
	 -0.3f, -0.5f,   0.0f, 0.0f,
	  0.3f, -0.5f,   1.0f, 0.0f,
	  0.3f,  0.5f,   1.0f, 1.0f,
	 -0.3f,  0.5f,   0.0f, 1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	uint32_t indices[] = {
		0, 1, 2,
		0, 2, 3
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	Texture2D* uvTexture = new Texture2D("assets/Textures/uv-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_UNSIGNED_BYTE);
	uvTexture->TexUnit(m_Shader, "u_Texture", 0);
}

void GameLayer::OnUpdate(float dt)
{
	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_Shader->Use();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

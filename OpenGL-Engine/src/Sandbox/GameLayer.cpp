#include "Sandbox/GameLayer.h"
#include <glad/glad.h>

const char* vertexSource = R"(
#version 330
precision highp float;

layout(location = 0) in vec2 a_Position;

out vec2 v_Position;

void main()
{
	gl_Position = vec4(a_Position, 0, 1);
	v_Position = a_Position;
}
)";

const char* fragmentSource = R"(
#version 330

out vec4 outColor;

in vec2 v_Position;

void main()
{
	outColor = vec4(v_Position * 0.5 + 0.5, 0, 1);
}
)";


GameLayer::GameLayer()
{
	m_Shader = new Shader(vertexSource, fragmentSource);

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

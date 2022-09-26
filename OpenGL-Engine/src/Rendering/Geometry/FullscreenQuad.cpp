#include "pch.h"

#include "Rendering/Geometry/FullscreenQuad.h"

#include <glad/glad.h>

FullscreenQuad::FullscreenQuad()
{
	float vertices[] = {
		 // NDC pos	// UV coords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

FullscreenQuad::~FullscreenQuad()
{
}

void FullscreenQuad::Render()
{
	glBindVertexArray(m_Vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

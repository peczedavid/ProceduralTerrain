#include "Rendering/Geometry/Plane.h"

Plane::Plane(uint32_t width, uint32_t div)
{
	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ebo);

	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	const float a = 0.5f;
	float verticesSquare[4 * 5] = {
		-a,  0,  a,  0, 0,    a,  0,  a, 1, 0,
		 a,  0, -a,  1, 1,   -a,  0, -a, 0, 1
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSquare), &verticesSquare[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	uint32_t indicesSquare[4] = {
		0, 1, 2, 3
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesSquare), &indicesSquare[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Plane::~Plane()
{
}

void Plane::GenerateMesh(uint32_t width, uint32_t div)
{
}

void Plane::Render()
{
	glBindVertexArray(m_Vao);
	glDrawElements(GL_PATCHES, 4, GL_UNSIGNED_INT, 0);
}

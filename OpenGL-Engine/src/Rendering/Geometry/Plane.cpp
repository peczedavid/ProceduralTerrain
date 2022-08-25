#include "Rendering/Geometry/Plane.h"

Plane::Plane(uint32_t width, uint32_t _div)
{
	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ebo);

	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	//  6---7---8
	//  |   |   |
	//  3---4---5
	//  |   |   |
	//  0---1---2

	constexpr float w = 2.0f;
	constexpr int div = 2;
	float verticesSquare[(2 + 1) * (2 + 1) * (3 + 2)] = {
		/*0*/ (float)0 / (float)div * w, 0, (float)0 / (float)div * w, (float)0 / div, (float)0 / div,
		/*1*/ (float)1 / (float)div * w, 0, (float)0 / (float)div * w, (float)1 / div, (float)0 / div,
		/*2*/ (float)2 / (float)div * w, 0, (float)0 / (float)div * w, (float)2 / div, (float)0 / div,

		/*3*/ (float)0 / (float)div * w, 0, (float)1 / (float)div * w, (float)0 / div, (float)1 / div,
		/*4*/ (float)1 / (float)div * w, 0, (float)1 / (float)div * w, (float)1 / div, (float)1 / div,
		/*5*/ (float)2 / (float)div * w, 0, (float)1 / (float)div * w, (float)2 / div, (float)1 / div,

		/*6*/ (float)0 / (float)div * w, 0, (float)2 / (float)div * w, (float)0 / div, (float)2 / div,
		/*7*/ (float)1 / (float)div * w, 0, (float)2 / (float)div * w, (float)1 / div, (float)2 / div,
		/*8*/ (float)2 / (float)div * w, 0, (float)2 / (float)div * w, (float)2 / div, (float)2 / div,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSquare), &verticesSquare[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	uint32_t indicesSquare[div * div * 4] = {
		// 0 * 3 + 0
		0, 0 + 1, 0 + div + 1 + 1, 0 + div + 1,
		// 0 * 3 + 1
		1, 1 + 1, 1 + div + 1 + 1, 1 + div + 1,
		// 1 * 3 + 0
		3, 3 + 1, 3 + div + 1 + 1, 3 + div + 1,
		// 1 * 3 + 1
		4, 4 + 1, 4 + div + 1 + 1, 4 + div + 1
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
	glDrawElements(GL_PATCHES, 16, GL_UNSIGNED_INT, 0);
}

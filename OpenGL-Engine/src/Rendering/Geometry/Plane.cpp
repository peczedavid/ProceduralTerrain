#include "pch.h"

#include "Rendering/Geometry/Plane.h"

Plane::Plane(uint32_t width, uint32_t div)
{
	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ebo);

	glBindVertexArray(m_Vao);
	

	this->GenerateMesh(width, div);

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
	//  6---7---8
	//  |   |   |
	//  3---4---5
	//  |   |   |
	//  0---1---2

	m_Vertices.clear();
	for (uint32_t y = 0; y <= div; y++)
	{
		for (uint32_t x = 0; x <= div; x++)
		{
			m_Vertices.push_back((float)x / (float)div * width);
			m_Vertices.push_back(0);
			m_Vertices.push_back((float)y / (float)div * width);
			m_Vertices.push_back((float)x / div);
			m_Vertices.push_back(1.0f - ((float)y / div));
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(float), &m_Vertices[0], GL_STATIC_DRAW);

	m_Indices.clear();
	for (uint32_t y = 0; y < div; y++)
	{
		for (uint32_t x = 0; x < div; x++)
		{
			uint32_t i = y * (div + 1) + x;
			m_Indices.push_back(i + div + 1);
			m_Indices.push_back(i + div + 1 + 1);
			m_Indices.push_back(i + 1);
			m_Indices.push_back(i);
			/*m_Indices.push_back(i);
			m_Indices.push_back(i + 1);
			m_Indices.push_back(i + div + 1 + 1);
			m_Indices.push_back(i + div + 1);*/
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint16_t), &m_Indices[0], GL_STATIC_DRAW);
}

void Plane::Render()
{
	glBindVertexArray(m_Vao);
	glDrawElements(GL_PATCHES, m_Indices.size(), GL_UNSIGNED_SHORT, 0);
}

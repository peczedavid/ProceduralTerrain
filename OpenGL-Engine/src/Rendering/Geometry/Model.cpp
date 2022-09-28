#include "pch.h"
#include "Rendering/Geometry/Model.h"

Model::Model(const std::string& path)
{
	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ebo);

	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

	constexpr float s = 0.5f;
	constexpr float vertices[24 * 8] = {
		// FAR FACE
		 s, -s, -s,  0,  0, -1, 0, 0,   -s, -s, -s,  0,  0, -1, 1, 0,   -s,  s, -s,  0,  0, -1, 1, 1,    s,  s, -s,  0,  0, -1, 0, 1,
		// CLOSE FACE															
		-s, -s,  s,  0,  0,  1, 0, 0,    s, -s,  s,  0,  0,  1, 1, 0,    s,  s,  s,  0,  0,  1, 1, 1,   -s,  s,  s,  0,  0,  1, 0, 1,
		// RIGHT FACE															
		 s, -s,  s,  1,  0,  0, 0, 0,    s, -s, -s,  1,  0,  0, 1, 0,    s,  s, -s,  1,  0,  0, 1, 1,    s,  s,  s,  1,  0,  0, 0, 1,
		// LEFT FACE															
		-s, -s, -s, -1,  0,  0, 0, 0,   -s, -s,  s, -1,  0,  0, 1, 0,   -s,  s,  s, -1,  0,  0, 1, 1,   -s,  s, -s, -1,  0,  0, 0, 1,
		// UP FACE																
		-s,  s,  s,  0,  1,  0, 0, 0,    s,  s,  s,  0,  1,  0, 1, 0,    s,  s, -s,  0,  1,  0, 1, 1,   -s,  s, -s,  0,  1,  0, 0, 1,
		// DOWN FACE															
		-s, -s, -s,  0, -1,  0, 0, 0,    s, -s, -s,  0, -1,  0, 1, 0,    s, -s,  s,  0, -1,  0, 1, 1,   -s, -s,  s,  0, -1,  0, 0, 1,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	constexpr uint32_t indices[36] = {
		 2,  1,  0,    3,  2,  0,
		 6,  5,  4,    7,  6,  4,
		10,  9,  8,   11, 10,  8,
		14, 13, 12,   15, 14, 12,
		18, 17, 16,   19, 18, 16,
		22, 21, 20,   23, 22, 20
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Model::~Model()
{
}

void Model::Draw(Ref<Shader> shader)
{
	glBindVertexArray(m_Vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

class Plane
{
public:
	Plane(uint32_t width, const uint32_t div);
	~Plane();

	void GenerateMesh(uint32_t width, const uint32_t div);
	void Render();
private:
	uint32_t m_Vao, m_Vbo, m_Ebo;
	std::vector<GLfloat> m_Vertices;
	std::vector<GLuint> m_Indices;
};

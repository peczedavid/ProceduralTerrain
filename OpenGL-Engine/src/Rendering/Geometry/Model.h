#pragma once

#include "Core/Core.h"
#include "Rendering/Shaders/Shader.h"

class Model
{
public:
	Model(const std::string& path);
	Model(const Model&) = delete;
	Model operator=(const Model&) = delete;
	~Model();

	void Draw(Ref<Shader> shader);
private:
	GLuint m_Vao, m_Vbo, m_Ebo;
	std::vector<glm::vec3> m_Vertices, m_Normals;
	std::vector<glm::vec2> m_TexCoords;
	std::vector<uint32_t> m_Indices;
};
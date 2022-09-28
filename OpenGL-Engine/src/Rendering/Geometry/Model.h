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
};
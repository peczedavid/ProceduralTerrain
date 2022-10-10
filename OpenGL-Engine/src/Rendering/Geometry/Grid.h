#pragma once

#include <glad/glad.h>
#include "Core/Core.h"
#include "Rendering/Shaders/Shader.h"

class Grid
{
public:
	Grid(const Ref<Shader> shader);

	void Draw();
private:
	GLuint m_Vao, m_Vbo;
	Shader* m_Shader;
};

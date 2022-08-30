#pragma once
#include <glad/glad.h>

#include "Rendering/Shaders/BasicShader.h"
#include "Rendering/Camera.h"

class Axis
{
public:
	Axis();
	~Axis();

	void Render(Camera* camera);
private:
	BasicShader* m_Shader;
	uint32_t m_Vao, m_Vbo;
	glm::mat4 m_Model;
};

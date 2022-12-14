#pragma once

#include <glad/glad.h>

#include "Rendering/Shaders/Shader.h"
#include "Rendering/Camera.h"
#include "Core/Core.h"

class Axis
{
public:
	Axis(Ref<Shader> m_Shader);
	~Axis();

	void Render(Ref<Camera> camera);
	Ref<Shader> m_Shader;
private:
	uint32_t m_Vao, m_Vbo;
	glm::mat4 m_Model;
};

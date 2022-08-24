#pragma once

#include <stdint.h>
#include "Rendering/Shaders/Shader.h"
#include "Rendering/Camera.h"

class Skybox
{
public:
	Skybox(Shader* shader);
	~Skybox();

	void Render(Camera* camera);
private:
	Shader* m_Shader;
	uint32_t m_Vao, m_Vbo, m_Ebo;
	uint32_t m_TextureId;
};

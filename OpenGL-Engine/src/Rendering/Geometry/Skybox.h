#pragma once

#include <stdint.h>
#include "Rendering/Shaders/ShaderBase.h"
#include "Rendering/Camera.h"

class Skybox
{
public:
	Skybox(ShaderBase* shader);
	~Skybox();

	void Render(Camera* camera);
	ShaderBase* m_Shader;
private:
	uint32_t m_Vao, m_Vbo, m_Ebo;
	uint32_t m_TextureId;
};

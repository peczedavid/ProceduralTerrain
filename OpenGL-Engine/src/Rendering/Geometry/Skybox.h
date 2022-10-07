#pragma once

#include <stdint.h>
#include "Rendering/Shaders/Shader.h"
#include "Rendering/Camera.h"
#include "Core/Core.h"

class Skybox
{
public:
	Skybox(Ref<Shader> shader);
	~Skybox();

	void Render();
	Ref<Shader> m_Shader;
private:
	uint32_t m_Vao, m_Vbo, m_Ebo;
	uint32_t m_TextureId;
};

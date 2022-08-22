#pragma once
#include "Core/Layers/Layer.h"
#include "Rendering/Shader.h"

class GameLayer : public Layer
{
public:
	GameLayer();

	void OnUpdate(float dt);
private:
	uint32_t m_Vao, m_Vbo, m_Ebo;
	Shader* m_Shader;
};

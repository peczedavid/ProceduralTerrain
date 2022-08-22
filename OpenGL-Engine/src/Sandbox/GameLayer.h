#pragma once
#include "Core/Layers/Layer.h"

class GameLayer : public Layer
{
public:
	GameLayer();

	void OnUpdate(float dt);
private:
	uint32_t m_Vao, m_Vbo, m_Ebo;
	uint32_t m_ShaderId;
};

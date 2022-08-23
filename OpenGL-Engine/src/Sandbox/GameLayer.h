#pragma once
#include "Core/Layers/Layer.h"
#include "Rendering/Shader.h"
#include "Rendering/Texture2D.h"
#include "Rendering/Camera.h"

class GameLayer : public Layer
{
public:
	GameLayer();

	void OnUpdate(float dt) override;
	void OnImGuiRender() override;
private:
	uint32_t m_Vao, m_Vbo, m_Ebo;
	Shader* m_Shader;
	Texture2D* m_UvTexture;
	Camera* m_Camera;
};

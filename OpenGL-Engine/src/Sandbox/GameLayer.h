#pragma once
#include "Core/Layers/Layer.h"
#include "Rendering/Shaders/Shader.h"
#include "Rendering/Shaders/ComputeShader.h"
#include "Rendering/Texture2D.h"
#include "Rendering/Camera.h"
#include "Rendering/Skybox.h"

class GameLayer : public Layer
{
public:
	GameLayer();

	void OnUpdate(float dt) override;
	void OnImGuiRender() override;
private:
	void GenerateHeightMap();
private:
	uint32_t m_Vao, m_Vbo, m_Ebo;
	Shader* m_Shader;
	//ComputeShader* m_ComputeShader;
	Texture2D* m_UvTexture, *m_HeightMap;
	Camera* m_Camera;
	Skybox* m_Skybox;
};

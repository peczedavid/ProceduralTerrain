#pragma once
#include "Core/Layers/Layer.h"
#include "Rendering/Shaders/Shader.h"
#include "Rendering/Texture2D.h"
#include "Rendering/Camera.h"
#include "Rendering/Skybox.h"
#include "Rendering/Shaders/TessellationShader.h"

class GameLayer : public Layer
{
public:
	GameLayer();

	void OnUpdate(float dt) override;
	void OnImGuiRender() override;
private:
	void GenerateHeightMap();
private:
	uint32_t m_VaoCube, m_VboCube, m_EboCube;
	uint32_t m_VaoSquare, m_VboSquare, m_EboSquare;
	int m_TessLevelInner = 1;
	int m_TessLevelOuter = 1;

	Shader* m_Shader;
	TessellationShader* m_TessellationShader;
	Texture2D* m_UvTexture, *m_HeightMap;
	Camera* m_Camera;
	Skybox* m_Skybox;
};

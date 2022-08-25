#pragma once
#include "Core/Layers/Layer.h"
#include "Rendering/Shaders/Shader.h"
#include "Rendering/Texture2D.h"
#include "Rendering/Camera.h"
#include "Rendering/Skybox.h"
#include "Rendering/Shaders/TessellationShader.h"
#include "Rendering/Geometry/Plane.h"

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
	Plane* m_Plane;
	int m_TessLevel = 1;
	float m_MaxHeight = 5.0f;

	Shader* m_Shader;
	TessellationShader* m_TessellationShader;
	Texture2D* m_UvTexture, *m_HeightMap;
	Camera* m_Camera;
	Skybox* m_Skybox;
};

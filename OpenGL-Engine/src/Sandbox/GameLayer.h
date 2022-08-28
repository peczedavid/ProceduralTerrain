#pragma once
#include "Core/Layers/Layer.h"
#include "Rendering/Shaders/BasicShader.h"
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
	float m_MaxHeight = 41.667f;

	BasicShader* m_Shader;
	TessellationShader* m_TessellationShader;
	Texture2D* m_UvTexture, * m_HeightMap;
	Texture2D *m_GroundTexture, *m_RockTexture, *m_SnowTexture;
	float m_GrassLevel = 0.25f, m_RockLevel = 0.64f, m_SnowLevel = 0.884f;
	Camera* m_Camera;
	Skybox* m_Skybox;

	float m_Gain = 0.432f, m_Lacunarity = 1.763f, m_Amplitude = 0.691f, m_Frequency = 0.171f;
	//float m_Gain = 0.5f, m_Lacunarity = 2.0f, m_Amplitude = 0.5f, m_Frequency = 2.0f;
	float m_Scale = 1.0f;

	glm::vec4 m_LightDir = glm::vec4(0.5f, 0.5f, 0.0f, 0.0f);

	float m_Mul = 1.0f, m_Div = 100.f, m_Hei = 165.0f;
};

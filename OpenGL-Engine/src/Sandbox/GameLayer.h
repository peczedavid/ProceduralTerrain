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
	float m_MaxHeight = 56.845;

	BasicShader* m_Shader;
	TessellationShader* m_TessellationShader;
	Texture2D* m_UvTexture;
	Texture2D *m_GroundTexture, *m_RockTexture, *m_SnowTexture;
	float m_GrassLevel = 0.0f, m_RockLevel = 0.094f, m_SnowLevel = 0.661f;
	Camera* m_Camera;
	Skybox* m_Skybox;

	float m_Gain = 0.245f, m_Lacunarity = 1.184f, m_Amplitude = 0.661f, m_Frequency = 2.776f;
	float m_Scale = 0.100f, m_HeightOffset = 20.726f;
	float m_FogDensity = 0.002f;
	float m_FogGradient = 2.5f;
	glm::vec2 m_NoiseOffset = glm::vec2(0.0f, 0.0f);
};

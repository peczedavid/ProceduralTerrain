#pragma once

#include "Core/Layers/Layer.h"
#include "Rendering/Shaders/BasicShader.h"
#include "Rendering/Texture2D.h"
#include "Rendering/Camera.h"
#include "Rendering/Skybox.h"
#include "Rendering/Shaders/TessellationShader.h"
#include "Rendering/Geometry/Plane.h"
#include "Rendering/Geometry/Axis.h"
#include "Rendering/FrameBuffer.h"
#include "Rendering/Geometry/FullscreenQuad.h"
#include "Rendering/Shaders/ComputeShader.h"

class GameLayer : public Layer
{
public:
	GameLayer();

	void OnUpdate(float dt) override;
	void OnImGuiRender(float dt) override;
private:
	void GenerateHeightMap();
	void RenderStart();
	void RenderEnd();
private:
	uint32_t m_VaoCube, m_VboCube, m_EboCube;
	Plane* m_Plane;
	int m_TessLevel = 1;
	float m_MaxHeight = 115.0f;

	ComputeShader* m_ComputeShader;

	BasicShader* m_Shader;
	TessellationShader* m_TerrainShader;
	TessellationShader* m_WaterShader;
	Texture2D* m_UvTexture;
	Texture2D *m_GroundTexture, *m_RockTexture, *m_SnowTexture;
	Texture2D* m_WaterTexture;
	float m_GrassLevel = 0.0f, m_RockLevel = 0.094f, m_SnowLevel = 0.661f;
	Camera* m_Camera;
	Skybox* m_Skybox;
	Axis* m_Axis;

	float m_Gain = 0.245f, m_Lacunarity = 1.184f, m_Amplitude = 0.27f, m_Frequency = 2.776f;
	float m_Scale = 0.100f, m_HeightOffset = 20.726f;
	float m_FogDensity = 0.002f;
	float m_FogGradient = 2.5f;
	glm::vec2 m_NoiseOffset = glm::vec2(0.0f, 0.0f);
	
	Plane* m_WaterPlane;
	float m_WaterLevel = -15.0f;
	glm::vec4 m_WaveA = glm::vec4(1.0f, 1.0f, 0.125f, 60.0f);
	glm::vec4 m_WaveB = glm::vec4(1.0f, 0.6f, 0.125f, 31.0f);
	glm::vec4 m_WaveC = glm::vec4(1.0f, 1.3f, 0.125f, 18.0f);

	bool m_TerrainNormals = false;
	bool m_WaterNormals = false;

	BasicShader* m_PostProcessShader;
	FrameBuffer* m_FrameBuffer;
	FullscreenQuad* m_FullscreenQuad;
};

#pragma once

#include "Core/Layers/Layer.h"
#include "Rendering/Texture2D.h"
#include "Rendering/Camera.h"
#include "Rendering/Geometry/Skybox.h"
#include "Rendering/Geometry/Plane.h"
#include "Rendering/Geometry/Axis.h"
#include "Rendering/FrameBuffer.h"
#include "Rendering/Geometry/FullscreenQuad.h"

class GameLayer : public Layer
{
	friend class GameLayerImGui;
public:
	GameLayer();

	void OnUpdate(float dt) override;
	void OnImGuiRender(float dt) override;

	void OnScreenshot() override;
private:
	void RenderStart();
	void RenderEnd();

	void GenerateTerrain();
	void UpdateFPS(float dt);
	void GenerateFFTTextures();
	void FFTLoop();
private:
	Plane* m_GroundPlane;
	int m_TessLevel = 1;
	float m_MaxHeight = 87.0f;

	Shader* m_TerrainComputeShader;
	std::vector<Texture2D*> m_HeightMaps;

	Shader* m_TerrainShader;
	Shader* m_WaterShader;
	Texture2D* m_GroundTexture, * m_RockTexture;
	Texture2D* m_WaterTexture;
	float m_GrassLevel = 0.0f, m_RockLevel = 0.094f, m_SnowLevel = 0.661f;
	Camera* m_Camera;
	Skybox* m_Skybox;
	Axis* m_Axis;

	float m_Gain = 0.245f, m_Lacunarity = 1.184f, m_Amplitude = 0.385f, m_Frequency = 2.776f;
	float m_Scale = 0.100f;
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

	Texture2D* m_H0k;
	Texture2D* m_H0minusk;
	Texture2D* m_HtDy, * m_HtDx, * m_HtDz;
	Shader* m_H0ComputeShader;
	Shader* m_HktComputeShader;
	Texture2D* m_TwiddleTexture;
	Shader* m_TwiddleShader;
	Texture2D* m_PingPong0, * m_PingPong1;
	Shader* m_ButterflyShader;
	Texture2D* m_Displacement;
	Shader* m_CopyShader;
	Shader* m_InversionShader;
	const uint32_t FFTResoltion = 256u;

	FrameBuffer* m_FrameBuffer;
	Shader* m_PostProcessShader;
	FullscreenQuad* m_FullscreenQuad;
	glm::vec2 m_ViewportSize = glm::vec2(1, 1);

	GameLayerImGui* m_UI;

	uint32_t m_FPS = 0u;
	float m_Time = 0.0f;
};

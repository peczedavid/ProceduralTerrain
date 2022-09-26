#pragma once

#include "Core/Core.h"
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
	ShaderLibrary m_ShaderLibrary;
	GLuint m_CameraUBO;

	Ref<Plane> m_GroundPlane;
	int m_TessLevel = 1;
	float m_MaxHeight = 87.0f;

	std::vector<Ref<Texture2D>> m_HeightMaps;

	Ref<Texture2D> m_GroundTexture, m_RockTexture;
	Ref<Texture2D> m_WaterTexture;
	float m_GrassLevel = 0.0f, m_RockLevel = 0.094f, m_SnowLevel = 0.661f;
	Ref<Camera> m_Camera;
	Ref<Skybox> m_Skybox;
	Ref<Axis> m_Axis;

	float m_Gain = 0.245f, m_Lacunarity = 1.184f, m_Amplitude = 0.385f, m_Frequency = 2.776f;
	float m_Scale = 0.100f;
	float m_FogDensity = 0.002f;
	float m_FogGradient = 2.5f;
	glm::vec2 m_NoiseOffset = glm::vec2(0.0f, 0.0f);

	Ref<Plane> m_WaterPlane;
	float m_WaterLevel = -15.0f;
	glm::vec4 m_WaveA = glm::vec4(1.0f, 1.0f, 0.125f, 60.0f);
	glm::vec4 m_WaveB = glm::vec4(1.0f, 0.6f, 0.125f, 31.0f);
	glm::vec4 m_WaveC = glm::vec4(1.0f, 1.3f, 0.125f, 18.0f);
	bool m_TerrainNormals = false;
	bool m_WaterNormals = false;

	Ref<Texture2D> m_H0k;
	Ref<Texture2D> m_H0minusk;
	Ref<Texture2D> m_HtDy, m_HtDx, m_HtDz;
	Ref<Texture2D> m_TwiddleTexture;
	Ref<Texture2D> m_PingPong0, m_PingPong1;
	Ref<Texture2D> m_Displacement;
	const uint32_t FFTResoltion = 256u;

	Ref<FrameBuffer> m_FrameBuffer;
	Ref<FullscreenQuad> m_FullscreenQuad;
	glm::vec2 m_ViewportSize = glm::vec2(1, 1);

	Scope<GameLayerImGui> m_UI;

	uint32_t m_FPS = 0u;
	float m_Time = 0.0f;
};

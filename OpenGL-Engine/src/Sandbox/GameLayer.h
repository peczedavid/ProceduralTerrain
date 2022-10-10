#pragma once

#include "Core/Core.h"
#include "Core/Layers/Layer.h"
#include "Rendering/Texture2D.h"
#include "Rendering/Camera.h"
#include "Rendering/Geometry/Skybox.h"
#include "Rendering/Geometry/Plane.h"
#include "Rendering/Geometry/Axis.h"
#include "Rendering/Geometry/Grid.h"
#include "Rendering/FrameBuffer.h"
#include "Rendering/Geometry/FullscreenQuad.h"
#include "Rendering/GameObject.h"
#include "Rendering/Material.h"

class GameLayer : public Layer
{
	friend class GameLayerImGui;
public:
	GameLayer();

	void OnUpdate(const float dt) override;
	void OnImGuiRender(const float dt) override;

	void OnScreenshot() override;
private:
	void RenderStart();
	void RenderEnd();

	void ToggleCamera();
	void SetUniformBuffers();
	void GenerateTerrain();
	void UpdateFPS(const float dt);
private:
	ShaderLibrary m_ShaderLibrary;
	GLuint m_CameraUBO;
	GLuint m_WavesUBO;
	GLuint m_EnviromentUBO;

	Ref<FPSCamera> m_Camera;
	Ref<TrackballCamera> m_TrackballCamera;
	Camera* m_ActiveCamera;
	uint32_t m_SelectedCamera = 1;
	Ref<Skybox> m_Skybox;
	Ref<Axis> m_Axis;
	Ref<Grid> m_Grid;

	Ref<Texture2D> m_UVTexture;
	Ref<Texture2D> m_GroundTexture, m_RockTexture, m_WaterTexture;
	std::vector<Ref<Texture2D>> m_HeightMaps;

	Ref<Plane> m_GroundPlane, m_WaterPlane;
	Ref<Model> m_MonkeyModel, m_TeapotModel, m_SphereModel, m_TreeModel;
	Ref<Model> m_ErenModel, m_ColossalModel, m_AmongUsModel;
	Ref<GameObject> m_Monkey, m_Teapot, m_Sphere, m_Tree, m_Eren, m_Colossal, m_AmongUs;
	std::unordered_map<std::string, Ref<GameObject>> m_GameObjects;

	glm::vec4 m_SunDirection = glm::vec4(0.254f, 0.341f, 0.905f, 0.0f);
	float m_Time = 0.0f;
	glm::vec2 m_FogData = glm::vec2(0.002f, 2.5f);
	glm::vec4 m_FogColor = glm::vec4(0.4f, 0.5f, 0.6f, 1.0f);

	int m_TessLevel = 1;
	float m_MaxHeight = 87.0f;
	bool m_TerrainNormals = false;
	bool m_ShadeTerrain = true;
	float m_GrassLevel = 0.0f, m_RockLevel = 0.094f, m_SnowLevel = 0.661f;

	float m_Gain = 0.245f, m_Lacunarity = 1.184f, m_Amplitude = 0.385f, m_Frequency = 2.776f;
	float m_Scale = 0.100f;
	
	glm::vec2 m_NoiseOffset = glm::vec2(0.0f, 0.0f);

	float m_WaterLevel = -25.0f;
	std::vector<glm::vec4> m_Waves;
	std::vector<glm::vec4> m_WavesInitial;
	const size_t m_WavesCount = 16;
	float m_WaterShininess = 140.0f, m_WaterReflectivity = 1.05f;
	float m_SteepnessDropoff = 0.496f, m_WavelengthDropoff = 0.138f;
	bool m_WaterNormals = false;

	Ref<FrameBuffer> m_FrameBuffer;
	Ref<FullscreenQuad> m_FullscreenQuad;
	glm::vec2 m_ViewportSize = glm::vec2(1, 1);

	Scope<GameLayerImGui> m_UI;

	uint32_t m_FPS = 0u;
};

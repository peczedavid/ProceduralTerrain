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

class ClothSimulationLayer : public Layer
{
	friend class ClothSimulationLayerImGui;
public:
	ClothSimulationLayer();

	void OnUpdate(const float dt) override;
	void OnImGuiRender(const float dt) override;
private:
	void RenderStart();
	void RenderEnd();

	void ToggleCamera();
	void SetUniformBuffers();
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

	Ref<Model> m_MonkeyModel, m_TeapotModel;
	std::unordered_map<std::string, Ref<GameObject>> m_GameObjects;

	glm::vec4 m_SunDirection = glm::vec4(0.254f, 0.341f, 0.905f, 0.0f);
	float m_Time = 0.0f;
	glm::vec2 m_FogData = glm::vec2(0.001f, 2.5f);
	glm::vec4 m_FogColor = glm::vec4(0.4f, 0.5f, 0.6f, 1.0f);

	Ref<FrameBuffer> m_FrameBuffer;
	Ref<FullscreenQuad> m_FullscreenQuad;
	glm::vec2 m_ViewportSize = glm::vec2(1, 1);

	Scope<ClothSimulationLayerImGui> m_UI;

	uint32_t m_FPS = 0u;
};

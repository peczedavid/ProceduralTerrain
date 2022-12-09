#include "pch.h"

#include "ClothSimulationLayer.h"
#include <Rendering/Renderer.h>
#include <Core/Application.h>
#include <Sandbox/ClothSimulation/ClothSimulationLayerImGui.h>

ClothSimulationLayer::ClothSimulationLayer()
{
	m_ShaderLibrary.Add("Basic shader", CreateShaderRef(
		"assets/GLSL/default.vert",
		"assets/GLSL/default.frag"
	));

	m_ShaderLibrary.Add("PBR shader", CreateShaderRef(
		"assets/GLSL/pbr/pbr.vert",
		"assets/GLSL/pbr/pbr.frag"
	));

	m_Camera = CreateRef<FPSCamera>(glm::vec3(0, 10, 30), glm::vec3(0, -0.45f, -1.0f));
	m_Camera->Resize(1, 1);
	m_TrackballCamera = CreateRef<TrackballCamera>(192.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	m_TrackballCamera->Resize(1, 1);
	ToggleCamera();


	auto skyboxShader = CreateShaderRef("assets/GLSL/skybox.vert", "assets/GLSL/skybox.frag");
	m_ShaderLibrary.Add("Skybox shader", skyboxShader);
	m_Skybox = CreateRef<Skybox>(skyboxShader);

	auto axisShader = CreateShaderRef("assets/GLSL/axis/axis.vert", "assets/GLSL/axis/axis.frag");
	m_ShaderLibrary.Add("Axis shader", axisShader);
	m_Axis = CreateRef<Axis>(axisShader);

	auto gridShader = CreateShaderRef("assets/GLSL/grid/grid.vert", "assets/GLSL/grid/grid.frag");
	m_ShaderLibrary.Add("Grid shader", gridShader);
	m_Grid = CreateRef<Grid>(gridShader);

	m_FullscreenQuad = CreateRef<FullscreenQuad>();

	m_UVTexture = CreateRef<Texture2D>("assets/Textures/uv-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	m_FrameBuffer = CreateRef<FrameBuffer>(1, 1);
	FrameBuffer::Default();

	m_UI = CreateScope<ClothSimulationLayerImGui>(this);

	glGenBuffers(1, &m_CameraUBO);
	const size_t cameraUBOsize = 3 * sizeof(glm::mat4) + sizeof(glm::vec4);
	glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);
	glBufferData(GL_UNIFORM_BUFFER, cameraUBOsize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_CameraUBO, 0, cameraUBOsize);

	glGenBuffers(1, &m_EnviromentUBO);
	constexpr size_t enviromentUBOSize = 2 * sizeof(glm::vec4) + sizeof(glm::vec2) + sizeof(float);
	glBindBuffer(GL_UNIFORM_BUFFER, m_EnviromentUBO);
	glBufferData(GL_UNIFORM_BUFFER, enviromentUBOSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 2, m_EnviromentUBO, 0, enviromentUBOSize);

	const auto& pbrShader = m_ShaderLibrary.Get("PBR shader");

	m_SphereModel = CreateRef<Model>("assets/Models/sphere.obj");

	Ref<PBRMaterial> whitePBRmaterial = CreateRef<PBRMaterial>();
	whitePBRmaterial->SetShader(pbrShader);
	Ref<PBRMaterial> greenPBRMaterial = CreateRef<PBRMaterial>();
	greenPBRMaterial->SetShader(pbrShader);
	greenPBRMaterial->Albedo = glm::vec3(0.35f, 0.6f, 0.15f);

	PlanetData planetData1;
	planetData1.Radius = 10.0f;
	planetData1.Mass = 300.0f;
	Ref<Planet> planet1 = CreateRef<Planet>(m_SphereModel.get(), whitePBRmaterial, planetData1);
	planet1->SetScale(10.0f);
	planet1->SetPosition(glm::vec3(-20.0f, 5.0f, 0.0f));

	PlanetData planetData2;
	planetData2.Radius = 10.0f;
	planetData2.Velocity = glm::vec3(0.0f, 0.0f, -1.0f);
	Ref<Planet> planet2 = CreateRef<Planet>(m_SphereModel.get(), greenPBRMaterial, planetData2);
	planet2->SetScale(10.0f);
	planet2->SetPosition(glm::vec3(20.0f, 5.0f, 0.0f));


	m_GameObjects["Planet 1"] = planet1;
	m_GameObjects["Planet 2"] = planet2;
	m_Planets.push_back(planet1);
	m_Planets.push_back(planet2);
}

void ClothSimulationLayer::OnUpdate(const float dt)
{
	RenderStart();

	UpdateFPS(dt);

	m_Time += dt;
	const float fov = 45.0f, const nearPlane = 0.1f, const farPlane = 3000.0f;

	m_ActiveCamera->CalculateMatrix(fov, nearPlane, farPlane);
	m_ActiveCamera->Update(dt);

	SetUniformBuffers();

	for (auto& planet : m_Planets)
		planet->UpdateVelocity(m_Planets, dt);

	for (auto& planet : m_Planets)
		planet->UpdatePosition(dt);

	for (auto& gameObject : m_GameObjects)
		gameObject.second->Draw();

	m_Skybox->Render();

	if (Renderer::DebugView)
		m_Axis->Render(m_ActiveCamera);

	if (Renderer::Grid)
		m_Grid->Draw();

	RenderEnd();
}

void ClothSimulationLayer::OnImGuiRender(const float dt)
{
	static bool show = true;
	ImGui::ShowDemoWindow(&show);
	m_UI->ViewportPanel();
	m_UI->ControlsPanel();
	m_UI->VendorInfoPanel();
	m_UI->GameObjectsPanel(); // FIRST
	m_UI->PropertiesPanel();  // SECOND
	m_UI->GraphicsSettingsPanel();
	m_UI->ShadersPanel();
	Log::Draw();
	if (Renderer::DebugView) m_UI->DebugOverlayPanel();
}

void ClothSimulationLayer::RenderStart()
{
	m_FrameBuffer->Bind();
	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void ClothSimulationLayer::RenderEnd()
{
	FrameBuffer::Default();
}

void ClothSimulationLayer::ToggleCamera()
{
	m_SelectedCamera = (m_SelectedCamera + 1) % 2;
	if (m_SelectedCamera == 0)
		m_ActiveCamera = m_Camera.get();
	else if (m_SelectedCamera == 1)
		m_ActiveCamera = m_TrackballCamera.get();

	Application::Get().SetCursor(m_SelectedCamera == 1);
}

void ClothSimulationLayer::SetUniformBuffers()
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::mat4), sizeof(glm::mat4), &m_ActiveCamera->GetView()[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), &m_ActiveCamera->GetProj()[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), &m_ActiveCamera->GetViewProj()[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), sizeof(glm::vec4), &m_ActiveCamera->GetPosition()[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, m_EnviromentUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 0, sizeof(float) * 4, &m_SunDirection[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 4, sizeof(float) * 4, &m_FogColor[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 8, sizeof(float) * 1, &m_FogData[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 9, sizeof(float) * 1, &m_FogData[1]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 10, sizeof(float) * 1, &m_Time);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ClothSimulationLayer::UpdateFPS(const float dt)
{
	static float lastFPSUpdate = 0.0f;
	static float sumDt = 0.0f;
	static uint32_t numFrames = 0u;
	lastFPSUpdate += dt;
	sumDt += dt;
	numFrames++;

	static float lastFPSSampleUpdate = 0.0f;
	lastFPSSampleUpdate += dt;
	if (lastFPSSampleUpdate >= 0.016f)
	{
		Renderer::FPSPool.AddFPSSample(static_cast<float>(m_FPS));
		lastFPSSampleUpdate = 0;
	}

	if (lastFPSUpdate >= 0.5f) {
		lastFPSUpdate = 0.0f;
		m_FPS = (float)(numFrames) / sumDt;
		numFrames = 0u;
		sumDt = 0.0f;
		auto& samples = Renderer::FPSPool.GetSamples();
		auto result = std::max_element(samples.begin(), samples.end());
		Renderer::FPSPool.SetMax(samples.at(std::distance(samples.begin(), result)));
	}
}

// TODO:
//		 - make renderer into static class			DONE
//		 - make FPSPool into own class/struct		DONE
//		 - put shaders into assets					DONE
//       - dist post build command					DONE
//		   (copy assets and imgui.ini)
//
//		 - uniforms in map							DONE
//		 - hot reaload shaders						DONE
// 		 - single shader class						DONE
// 		 - shader library							DONE
//		 - reference								DONE
// 
//		 - precompiled header						DONE
//		 - logging									DONE
//		 - macros for logging, assert				DONE
//		 - uniform buffer objects eg camera uniform DONE
// 
//       - object loading							DONE
//		 - logging to imgui window					DONE
//			
//		 - nice gerstner waves						DONE
// 
//		 - guizmos									DONE
//		 - trackball camera controls				DONE
//		 https://computergraphics.stackexchange.com/questions/151/how-to-implement-a-trackball-in-opengl
//		 https://physicscatalyst.com/graduation/wp-content/uploads/2016/09/rectangular-to-spherical-coordinates-300x256.png
//		 - fix seames between chunks				DONE
//		 - infinite grid, CTRL+G toggle				DONE
//		 http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/
//		
//		 - rewrite controls panel
//		 - materials, pbr rendering?				WIP    - optional textures
//		 - tree rendering							WIP    - submesh system
// 
//		 - scene system (switching between scenes)
//		 - profiling (maybe benchmark scene)
//		 - screenshots in dist mode

//		 - ett, camera system
//		 https://www.3dgep.com/understanding-the-view-matrix/
//		 - procedural sky with sun
// 
//		 - custom title bar
//		 https://kubyshkin.name/posts/win32-window-custom-title-bar-caption/
//		 https://www.youtube.com/watch?v=qZZePCNLBuQ
//		 https://github.com/grassator/win32-window-custom-titlebar

#include "pch.h"

#include "Sandbox/GameLayer.h"
#include "Core/Application.h"
#include "Rendering/Renderer.h"
#include "Sandbox/GameLayerImGui.h"

#include <imgui.h>
#include <glad/glad.h>

constexpr uint32_t planeSize = 1024u;
constexpr uint32_t planeDivision = 25u;
constexpr uint32_t waterPlaneSize = 1024;
constexpr uint32_t waterPlaneDivision = 25u;

float Random(float max = 1.0f, float min = 0.0f)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

GameLayer::GameLayer()
{
	m_ShaderLibrary.Add("Terrain shader", CreateShaderRef(
		"assets/GLSL/terrain/terrain.vert",
		"assets/GLSL/terrain/terrain.tesc",
		"assets/GLSL/terrain/terrain.tese",
		"assets/GLSL/terrain/terrain.frag"));
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	m_ShaderLibrary.Add("Water shader", CreateShaderRef(
		"assets/GLSL/water/water.vert",
		"assets/GLSL/water/water.tesc",
		"assets/GLSL/water/water.tese",
		"assets/GLSL/water/water.frag"));
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	m_ShaderLibrary.Add("Basic shader", CreateShaderRef(
		"assets/GLSL/default.vert",
		"assets/GLSL/default.frag"
	));

	m_ShaderLibrary.Add("PBR shader", CreateShaderRef(
		"assets/GLSL/pbr/pbr.vert",
		"assets/GLSL/pbr/pbr.frag"
	));

	// https://www.geeks3d.com/hacklab/20180514/demo-wireframe-shader-opengl-3-2-and-opengl-es-3-1/
	m_ShaderLibrary.Add("PBR wireframe shader", CreateShaderRef(
		"assets/GLSL/pbr/pbr-wireframe.vert",
		"assets/GLSL/pbr/pbr-wireframe.geom",
		"assets/GLSL/pbr/pbr-wireframe.frag"
	));

	m_GroundPlane = CreateRef<Plane>(planeSize, planeDivision);
	m_WaterPlane = CreateRef<Plane>(waterPlaneSize, waterPlaneDivision);

	m_Camera = CreateRef<FPSCamera>(glm::vec3(0, 64, 0), glm::vec3(0, -0.45f, -1.0f));
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

	m_GroundTexture = CreateRef<Texture2D>("assets/Textures/ground-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_RockTexture = CreateRef<Texture2D>("assets/Textures/rock-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_SnowTexture = CreateRef<Texture2D>("assets/Textures/snow-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_WaterTexture = CreateRef<Texture2D>("assets/Textures/water-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_UVTexture = CreateRef<Texture2D>("assets/Textures/uv-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	m_FrameBuffer = CreateRef<FrameBuffer>(1, 1);
	m_ShaderLibrary.Add("Post-process shader", CreateShaderRef(
		"assets/GLSL/postprocess.vert",
		"assets/GLSL/postprocess.frag"));
	FrameBuffer::Default();

	auto terrainComputeShader = CreateShaderRef("assets/GLSL/noise.comp");
	m_ShaderLibrary.Add("Terrain compute shader", terrainComputeShader);
	terrainComputeShader->Use();
	terrainComputeShader->SetUniform("u_Amplitude", m_Amplitude);
	terrainComputeShader->SetUniform("u_Gain", m_Gain);
	terrainComputeShader->SetUniform("u_Frequency", m_Frequency);
	terrainComputeShader->SetUniform("u_Scale", m_Scale);
	terrainComputeShader->SetUniform("u_NoiseOffset", m_NoiseOffset);

	for (int z = -1; z <= 1; z++)
		for (int x = -1; x <= 1; x++)
			m_HeightMaps.push_back(CreateRef<Texture2D>(planeSize, planeSize, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGBA32F));

	GenerateTerrain();

	m_UI = CreateScope<GameLayerImGui>(this);

	glGenBuffers(1, &m_CameraUBO);
	const size_t cameraUBOsize = 3 * sizeof(glm::mat4) + sizeof(glm::vec4);
	glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);
	glBufferData(GL_UNIFORM_BUFFER, cameraUBOsize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_CameraUBO, 0, cameraUBOsize);

	glGenBuffers(1, &m_WavesUBO);
	const size_t wavesUBOsize = m_WavesCount * sizeof(glm::vec4);
	glBindBuffer(GL_UNIFORM_BUFFER, m_WavesUBO);
	glBufferData(GL_UNIFORM_BUFFER, wavesUBOsize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_WavesUBO, 0, wavesUBOsize);

	glBindBuffer(GL_UNIFORM_BUFFER, m_WavesUBO);
	m_WavesInitial.resize(m_WavesCount);
	m_Waves.resize(m_WavesCount);
	for (size_t i = 0; i < m_WavesCount; i++)
	{
		const float steepness = Random(0.20f, 0.05f) / (i + 1);
		const float wavelength = Random(65.0f, 45.f) / (i + 1);
		m_WavesInitial[i] = glm::vec4(Random(1.0f), Random(1.0f), steepness, wavelength);
		glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::vec4), sizeof(glm::vec4), &m_WavesInitial[i][0]);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &m_EnviromentUBO);
	constexpr size_t enviromentUBOSize = 2 * sizeof(glm::vec4) + sizeof(glm::vec2) + sizeof(float);
	glBindBuffer(GL_UNIFORM_BUFFER, m_EnviromentUBO);
	glBufferData(GL_UNIFORM_BUFFER, enviromentUBOSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 2, m_EnviromentUBO, 0, enviromentUBOSize);

	m_SphereModel = CreateRef<Model>("assets/Models/sphere.obj");
	m_TeapotModel = CreateRef<Model>("assets/Models/teapot.obj");
	m_MonkeyModel = CreateRef<Model>("assets/Models/monkey.obj");
	m_TreeModel = CreateRef<Model>("assets/Models/birch_tree.obj");
	m_ErenModel = CreateRef<Model>("assets/Models/eren.obj");
	m_ColossalModel = CreateRef<Model>("assets/Models/colossal.obj");
	m_AmongUsModel = CreateRef<Model>("assets/Models/amogus.obj");

	m_GumsModel = CreateRef<Model>("assets/Models/gums.obj");
	m_TeethModel = CreateRef<Model>("assets/Models/teeth.obj");
	m_BracesModel = CreateRef<Model>("assets/Models/braces.obj");

	const auto pbrShader = m_ShaderLibrary.Get("PBR shader");

	Ref<PBRMaterial> gumsMaterial = CreateRef<PBRMaterial>();
	gumsMaterial->SetShader(pbrShader);
	gumsMaterial->Albedo = glm::vec3(1.0f, 0.0f, 0.04f);
	gumsMaterial->Roughness = 0.34f;
	gumsMaterial->Metallic = 0.0f;
	gumsMaterial->AmbientOcclusion = 0.3f;
	gumsMaterial->F0 = 0.04f;
	Ref<PBRMaterial> teethMaterial = CreateRef<PBRMaterial>();
	teethMaterial->SetShader(pbrShader);
	teethMaterial->Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	teethMaterial->Roughness = 0.34f;
	teethMaterial->Metallic = 0.0f;
	teethMaterial->AmbientOcclusion = 0.19f;
	teethMaterial->F0 = 0.04f;
	Ref<PBRMaterial> bracesMaterial = CreateRef<PBRMaterial>();
	bracesMaterial->SetShader(pbrShader);
	bracesMaterial->Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	bracesMaterial->Roughness = 0.2f;
	bracesMaterial->Metallic = 0.08f;
	bracesMaterial->AmbientOcclusion = 1.0f;
	bracesMaterial->F0 = 0.135f;

	Ref<PBRMaterial> whitePBRmaterial = CreateRef<PBRMaterial>();
	whitePBRmaterial->SetShader(pbrShader);
	Ref<PBRMaterial> redPBRMaterial = CreateRef<PBRMaterial>();
	redPBRMaterial->SetShader(pbrShader);
	redPBRMaterial->Albedo = glm::vec3(1, 0, 0);
	Ref<PBRMaterial> textuerPBRMaterial = CreateRef<PBRMaterial>();
	textuerPBRMaterial->SetShader(pbrShader);
	textuerPBRMaterial->UseAlbedoMap = true;
	textuerPBRMaterial->AlbedoMap = m_WaterTexture;

	m_Sphere = CreateRef<GameObject>(m_SphereModel.get(), textuerPBRMaterial);
	m_Sphere->SetPosition(glm::vec3(30.0f, 50.0f, -50.0f));
	m_Sphere->SetScale(10.0f);
	m_Teapot = CreateRef<GameObject>(m_TeapotModel.get(), whitePBRmaterial);
	m_Teapot->SetPosition(glm::vec3(-30.0f, 50.0f, -50.0f));
	m_Monkey = CreateRef<GameObject>(m_MonkeyModel.get(), whitePBRmaterial);
	m_Monkey->SetPosition(glm::vec3(0.0f, 50.0f, -50.0f));
	m_Monkey->SetScale(10.0f);
	m_Tree = CreateRef<GameObject>(m_TreeModel.get(), whitePBRmaterial);
	m_Tree->SetPosition(glm::vec3(-50.0f, 50.0f, -50.0f));
	m_Tree->SetScale(15.0f);
	m_Eren = CreateRef<GameObject>(m_ErenModel.get(), whitePBRmaterial);
	m_Eren->SetPosition(glm::vec3(-50.0f, 50.0f, -125.0f));
	m_Eren->SetScale(15.0f);
	m_Colossal = CreateRef<GameObject>(m_ColossalModel.get(), redPBRMaterial);
	m_Colossal->SetPosition(glm::vec3(50.0f, 50.0f, -175.0f));
	m_Colossal->SetScale(2.0f);
	m_AmongUs = CreateRef<GameObject>(m_AmongUsModel.get(), redPBRMaterial);
	m_AmongUs->SetPosition(glm::vec3(0.0f, 50.0f, -100.0f));
	m_AmongUs->SetScale(10.0f);

	m_GumsBottom = CreateRef<GameObject>(m_GumsModel.get(), gumsMaterial);
	m_GumsBottom->SetRotation(glm::vec3(0.0f, 3.14f, 0.0f));
	m_GumsTop = CreateRef<GameObject>(m_GumsModel.get(), gumsMaterial);
	m_GumsTop->SetRotation(glm::vec3(0.0f, 3.14f, 3.14f));
	m_TeethBottom = CreateRef<GameObject>(m_TeethModel.get(), teethMaterial);
	m_TeethBottom->SetRotation(glm::vec3(0.0f, 3.14f, 0.0f));
	m_TeethTop = CreateRef<GameObject>(m_TeethModel.get(), teethMaterial);
	m_TeethTop->SetRotation(glm::vec3(0.0f, 3.14f, 3.14f));
	m_BracesBottom = CreateRef<GameObject>(m_BracesModel.get(), bracesMaterial);
	m_BracesBottom->SetRotation(glm::vec3(0.0f, 3.14f, 0.0f));
	m_BracesTop = CreateRef<GameObject>(m_BracesModel.get(), bracesMaterial);
	m_BracesTop->SetRotation(glm::vec3(0.0f, 3.14f, 3.14f));
	
	m_MouthBase = CreateRef<GameObject>(nullptr, nullptr);
	m_MouthBase->SetScale(15.0f);
	m_MouthBase->SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));
	m_MouthBase->SetRotation(glm::vec3(0.0f, 3.14f, 0.0f));

	m_GameObjects["Sphere"] = m_Sphere;
	m_GameObjects["Tree"] = m_Tree;
	m_GameObjects["Monkey"] = m_Monkey;
	m_GameObjects["Teapot"] = m_Teapot;
	m_GameObjects["Eren"] = m_Eren;
	m_GameObjects["Colossal"] = m_Colossal;
	m_GameObjects["Among us"] = m_AmongUs;
	m_GameObjects["Gums bottom"] = m_GumsBottom;
	m_GameObjects["Gums top"] = m_GumsTop;
	m_GameObjects["Teeth bottom"] = m_TeethBottom;
	m_GameObjects["Teeth top"] = m_TeethTop;
	m_GameObjects["Braces bottom"] = m_BracesBottom;
	m_GameObjects["Braces top"] = m_BracesTop;
	m_GameObjects["Mouth base"] = m_MouthBase;
}

void GameLayer::OnUpdate(const float dt)
{
	RenderStart();

	UpdateFPS(dt);

	m_Time += dt;
	const float fov = 45.0f, const nearPlane = 0.1f, const farPlane = 3000.0f;

	m_ActiveCamera->CalculateMatrix(fov, nearPlane, farPlane);
	m_ActiveCamera->Update(dt);

	SetUniformBuffers();

	m_Skybox->Render();

	glm::mat4 model;

#if 1
	auto terrainShader = m_ShaderLibrary.Get("Terrain shader");
	terrainShader->Use();
	m_GroundTexture->Bind(1);
	m_RockTexture->Bind(2);
	m_SnowTexture->Bind(3);
	terrainShader->SetUniform("u_MaxLevel", m_MaxHeight);
	terrainShader->SetUniform("u_MaxLevel", m_MaxHeight);
	terrainShader->SetUniform("u_NormalView", m_TerrainNormals ? 1 : 0);
	terrainShader->SetUniform("u_Shade", m_ShadeTerrain ? 1 : 0);

	for (int z = -1; z <= 1; z++)
	{
		for (int x = -1; x <= 1; x++)
		{
			int index = (z + 1) * 3 + (x + 1);
			m_HeightMaps[index]->Bind(0);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(x * (int)planeSize, 0, z * (int)planeSize));
			terrainShader->SetUniform("u_Model", model);
			m_GroundPlane->Render();
		}
	}
#endif

	m_BracesBottom->SetScale(m_MouthBase->GetScale());
	m_BracesTop->SetScale(m_MouthBase->GetScale());
	m_TeethBottom->SetScale(m_MouthBase->GetScale());
	m_TeethTop->SetScale(m_MouthBase->GetScale());
	m_GumsBottom->SetScale(m_MouthBase->GetScale());
	m_GumsTop->SetScale(m_MouthBase->GetScale());
	const glm::vec3 offset(0.0f, m_MouthBase->GetScale().y, 0.0f);
	m_BracesBottom->SetPosition(m_MouthBase->GetPosition());
	m_BracesTop->SetPosition(m_MouthBase->GetPosition() + offset);
	m_TeethBottom->SetPosition(m_MouthBase->GetPosition());
	m_TeethTop->SetPosition(m_MouthBase->GetPosition() + offset);
	m_GumsBottom->SetPosition(m_MouthBase->GetPosition());
	m_GumsTop->SetPosition(m_MouthBase->GetPosition() + offset);

	m_GumsBottom->SetRotation(glm::vec3(-m_MouthRotation, 3.14f, 0.0f));
	m_GumsTop->SetRotation(glm::vec3(m_MouthRotation, 3.14f, 3.14f));
	m_TeethBottom->SetRotation(glm::vec3(-m_MouthRotation, 3.14f, 0.0f));
	m_TeethTop->SetRotation(glm::vec3(m_MouthRotation, 3.14f, 3.14f));
	m_BracesBottom->SetRotation(glm::vec3(-m_MouthRotation, 3.14f, 0.0f));
	m_BracesTop->SetRotation(glm::vec3(m_MouthRotation, 3.14f, 3.14f));

	for (auto& gameObject : m_GameObjects)
		gameObject.second->Draw();

#if 1
	auto waterShader = m_ShaderLibrary.Get("Water shader");
	waterShader->Use();
	m_WaterTexture->Bind(0);
	waterShader->SetUniform("u_Shininess", m_WaterShininess);
	waterShader->SetUniform("u_Reflectivity", m_WaterReflectivity);
	waterShader->SetUniform("u_NormalView", m_WaterNormals ? 1 : 0);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)waterPlaneSize / 2.f, m_WaterLevel, -(float)waterPlaneSize / 2.f));
	waterShader->SetUniform("u_Model", model);
	m_WaterPlane->Render();
#endif

	if (Renderer::DebugView)
		m_Axis->Render(m_ActiveCamera);

	if (Renderer::Grid)
		m_Grid->Draw();

	RenderEnd();
}

void GameLayer::OnImGuiRender(const float dt)
{
	static bool show = true;
	ImGui::ShowDemoWindow(&show);
	m_UI->ContolsPanel();
	m_UI->NoisePanel();
	m_UI->LandscapePanel();
	m_UI->WaterPanel();
	m_UI->ViewportPanel();
	m_UI->TexturesPanel();
	m_UI->VendorInfoPanel();
	m_UI->GameObjectsPanel(); // FIRST
	m_UI->PropertiesPanel();  // SECOND
	m_UI->GraphicsSettingsPanel();
	m_UI->ShadersPanel();
	m_UI->EnviromentPanel();
	Log::Draw();
	if (Renderer::DebugView) m_UI->DebugOverlayPanel();
}

void GameLayer::OnScreenshot()
{
	uint8_t* data = new uint8_t[m_ViewportSize.y * m_ViewportSize.x * 3];
	m_FrameBuffer->Bind();
	glPixelStorei(GL_PACK_ALIGNMENT, 1); // OpenGL memory to RAM, 1 byte per color channel
	glReadPixels(0, 0, m_ViewportSize.x, m_ViewportSize.y, GL_RGB, GL_UNSIGNED_BYTE, data);
	FrameBuffer::Default();
	const std::time_t t = std::time(0);
	const std::tm* now = std::localtime(&t);
	std::ostringstream oss;
	oss << "screenshots/"
		<< (now->tm_year + 1900) << '-'
		<< (now->tm_mon + 1) << '-'
		<< now->tm_mday << '_'
		<< now->tm_hour << '-'
		<< now->tm_min << '-'
		<< now->tm_sec
		<< ".png";
	const std::string title = oss.str();
	stbi_flip_vertically_on_write(1);
	stbi_write_png(title.c_str(), m_ViewportSize.x, m_ViewportSize.y, 3, data, m_ViewportSize.x * 3);
	delete[] data;
	delete now;
}

void GameLayer::RenderStart()
{
	m_FrameBuffer->Bind();
	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void GameLayer::RenderEnd()
{
	FrameBuffer::Default();
#ifdef POST_PROCESS
	m_PostProcessShader->Use();
	m_PostProcessShader->SetUniform("u_Orientation", m_Camera->GetOrientation());
	bool wireframe = Renderer::wireframe;
	if (Renderer::wireframe) Renderer::TogglePolygonMode();
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FrameBuffer->GetTextureId());
	m_FullscreenQuad->Render();
	if (wireframe) Renderer::TogglePolygonMode();
#endif
}

void GameLayer::ToggleCamera()
{
	m_SelectedCamera = (m_SelectedCamera + 1) % 2;
	if (m_SelectedCamera == 0)
		m_ActiveCamera = m_Camera.get();
	else if (m_SelectedCamera == 1)
		m_ActiveCamera = m_TrackballCamera.get();

	Application::Get().SetCursor(m_SelectedCamera == 1);
}

void GameLayer::SetUniformBuffers()
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::mat4), sizeof(glm::mat4), &m_ActiveCamera->GetView()[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), &m_ActiveCamera->GetProj()[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), &m_ActiveCamera->GetViewProj()[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), sizeof(glm::vec4), &m_ActiveCamera->GetPosition()[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, m_WavesUBO);
	for (size_t i = 0; i < m_WavesCount; i++)
	{
		const float steepness = std::max(m_WavesInitial[i].z * (m_SteepnessDropoff * i), 0.0001f);
		const float wavelength = std::max(m_WavesInitial[i].w * (m_WavelengthDropoff * i), 0.0001f);
		m_Waves[i] = glm::vec4(m_WavesInitial[i].x, m_WavesInitial[i].y, steepness, wavelength);
		glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::vec4), sizeof(glm::vec4), &m_Waves[i][0]);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, m_EnviromentUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 0, sizeof(float) * 4, &m_SunDirection[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 4, sizeof(float) * 4, &m_FogColor[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 8, sizeof(float) * 1, &m_FogData[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 9, sizeof(float) * 1, &m_FogData[1]);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float) * 10, sizeof(float) * 1, &m_Time);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GameLayer::GenerateTerrain()
{
	auto terrainComputeShader = m_ShaderLibrary.Get("Terrain compute shader");
	terrainComputeShader->Use();
	terrainComputeShader->SetUniform("u_Amplitude", m_Amplitude);
	terrainComputeShader->SetUniform("u_Gain", m_Gain);
	terrainComputeShader->SetUniform("u_Frequency", m_Frequency);
	terrainComputeShader->SetUniform("u_Scale", m_Scale);
	terrainComputeShader->SetUniform("u_NoiseOffset", m_NoiseOffset);

	for (int z = -1; z <= 1; z++)
	{
		for (int x = -1; x <= 1; x++)
		{
			const int index = (z + 1) * 3 + (x + 1);
			const uint32_t width = m_HeightMaps[index]->GetWidth();
			const uint32_t height = m_HeightMaps[index]->GetHeight();
			terrainComputeShader->SetUniform("u_WorldOffset", glm::vec2(x * ((int)width - 1), z * ((int)height - 1)));
			m_HeightMaps[index]->BindImage();
			terrainComputeShader->Dispatch(glm::uvec3(ceil(width / 16), ceil(height / 16), 1));
		}
	}
}

void GameLayer::UpdateFPS(const float dt)
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
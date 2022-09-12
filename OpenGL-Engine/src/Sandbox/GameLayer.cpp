// TODO: - offset terrain height relative to amplitude so it starts at 0
//		 - fix seames between chunks (maybe (planeSize+1)*(planeSize+1) heightmaps)

//		 - axis, fix length (use proj only?)

#include "Sandbox/GameLayer.h"
#include "Core/Application.h"
#include <glad/glad.h>
#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include <iostream>
#include <random>
#include "Rendering/Renderer.h"
#include <Windows.h>
#include <dxgi1_4.h>
#include <stb_image_write.h>

constexpr uint32_t planeSize = 1024u;
constexpr uint32_t planeDivision = 25u;
constexpr uint32_t waterPlaneSize = 1000u;
constexpr uint32_t waterPlaneDivision = 100u;

GameLayer::GameLayer()
{
	m_Shader = new BasicShader("src/Rendering/Shaders/glsl/default.vert", "src/Rendering/Shaders/glsl/default.frag");
	m_Shader->TexUnit("u_Texture", 0);
	m_TerrainShader = new TessellationShader(
		"src/Rendering/Shaders/glsl/terrain/terrain.vert",
		"src/Rendering/Shaders/glsl/terrain/terrain.tesc",
		"src/Rendering/Shaders/glsl/terrain/terrain.tese",
		"src/Rendering/Shaders/glsl/terrain/terrain.frag");
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	m_TerrainShader->TexUnit("u_NoiseTexture", 0);
	m_TerrainShader->TexUnit("u_GroundTexture", 1);
	m_TerrainShader->TexUnit("u_RockTexture", 2);
	m_TerrainShader->TexUnit("u_SnowTexture", 3);

	m_WaterShader = new TessellationShader(
		"src/Rendering/Shaders/glsl/water/water.vert",
		"src/Rendering/Shaders/glsl/water/water.tesc",
		"src/Rendering/Shaders/glsl/water/water.tese",
		"src/Rendering/Shaders/glsl/water/water.frag");
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	m_WaterShader->TexUnit("u_WaterTexture", 0);

	m_GroundPlane = new Plane(planeSize, planeDivision);
	m_WaterPlane = new Plane(waterPlaneSize, waterPlaneDivision);

	m_Camera = new Camera(glm::vec3(0, 64, 0), glm::vec3(0, -0.45f, -1.0f));
	m_Camera->Resize(1, 1);

	BasicShader* skyboxShader = new BasicShader("src/Rendering/Shaders/glsl/skybox.vert", "src/Rendering/Shaders/glsl/skybox.frag");
	m_Skybox = new Skybox(skyboxShader);

	m_Axis = new Axis();

	m_FullscreenQuad = new FullscreenQuad();

	m_GroundTexture = new Texture2D("assets/Textures/ground-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_RockTexture = new Texture2D("assets/Textures/rock-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_WaterTexture = new Texture2D("assets/Textures/water-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	m_FrameBuffer = new FrameBuffer(1, 1);
	m_PostProcessShader = new BasicShader(
		"src/Rendering/Shaders/glsl/postprocess.vert",
		"src/Rendering/Shaders/glsl/postprocess.frag"
	);
	m_PostProcessShader->TexUnit("u_ScreenTexture", 0);
	FrameBuffer::Default();

	m_ComputeShader = new ComputeShader("src/Rendering/Shaders/glsl/noise.comp");
	m_ComputeShader->Use();
	m_ComputeShader->SetUniform("u_Amplitude", m_Amplitude);
	m_ComputeShader->SetUniform("u_Gain", m_Gain);
	m_ComputeShader->SetUniform("u_Frequency", m_Frequency);
	m_ComputeShader->SetUniform("u_Scale", m_Scale);
	m_ComputeShader->SetUniform("u_NoiseOffset", m_NoiseOffset);

	for (int z = -1; z <= 1; z++)
		for (int x = -1; x <= 1; x++)
			m_HeightMaps.push_back(new Texture2D(planeSize, planeSize, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGBA32F));

	this->GenerateTerrain();
}

void GameLayer::OnUpdate(float dt)
{
	RenderStart();

	static float t = 0.0f; t += dt;
	float fov = 45.0f, nearPlane = 0.1f, farPlane = 3000.0f;

	m_Camera->UpdateMatrix(fov, nearPlane, farPlane);
	if (!Application::Get().IsCursor())
		m_Camera->Update(dt);

	m_TerrainShader->Use();
	m_TerrainShader->SetUniform("u_ViewProj", m_Camera->GetMatrix());
	m_Shader->Use();
	m_Shader->SetUniform("u_ViewProj", m_Camera->GetMatrix());

	m_Skybox->Render(m_Camera);

	glm::mat4 model = glm::mat4(1.0f);

	m_TerrainShader->Use();
	m_GroundTexture->Bind(1);
	m_RockTexture->Bind(2);
	m_TerrainShader->SetUniform("u_MaxLevel", m_MaxHeight);
	m_TerrainShader->SetUniform("u_View", m_Camera->GetView());
	m_TerrainShader->SetUniform("u_HeightOffset", m_HeightOffset);
	m_TerrainShader->SetUniform("u_FogGradient", m_FogGradient);
	m_TerrainShader->SetUniform("u_FogDensity", m_FogDensity);
	m_TerrainShader->SetUniform("u_NormalView", m_TerrainNormals ? 1 : 0);

#if 1
	for (int z = -1; z <= 1; z++)
	{
		for (int x = -1; x <= 1; x++)
		{
			int index = (z + 1) * 3 + (x + 1);
			m_HeightMaps[index]->Bind(0);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(x * (int)planeSize, 0, z * (int)planeSize));
			m_TerrainShader->SetUniform("u_Model", model);
			m_GroundPlane->Render();
		}
	}
#else
	//m_ComputeShader->GetTexture()->Bind(0);
	m_HeightMap1->Bind(0);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	m_TerrainShader->SetUniform("u_Model", model);
	m_Plane->Render();
	m_HeightMap2->Bind(0);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(planeSize, 0, 0));
	m_TerrainShader->SetUniform("u_Model", model);
	m_Plane->Render();
#endif

	/*m_WaterShader->Use();
	m_WaterTexture->Bind(0);
	m_WaterShader->SetUniform("u_ViewProj", m_Camera->GetMatrix());
	m_WaterShader->SetUniform("u_View", m_Camera->GetView());
	m_WaterShader->SetUniform("u_FogGradient", m_FogGradient);
	m_WaterShader->SetUniform("u_FogDensity", m_FogDensity);
	m_WaterShader->SetUniform("u_WaveA", m_WaveA);
	m_WaterShader->SetUniform("u_WaveB", m_WaveB);
	m_WaterShader->SetUniform("u_WaveC", m_WaveC);
	m_WaterShader->SetUniform("u_Time", t);
	m_WaterShader->SetUniform("u_NormalView", m_WaterNormals ? 1 : 0);*/

#if 0
	for (int z = -(levelSize - 2); z < (levelSize - 1); z++)
	{
		for (int x = -(levelSize - 2); x < (levelSize - 1); x++)
		{
			model = glm::translate(glm::mat4(1.0f), glm::vec3(x * (int)waterPlaneSize, m_WaterLevel, z * (int)waterPlaneSize));
			m_WaterShader->SetUniform("u_Model", model);
			m_WaterPlane->Render();
		}
	}
#else
	//model = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)waterPlaneSize / 2.f, m_WaterLevel, -(float)waterPlaneSize / 2.f));
	//m_WaterShader->SetUniform("u_Model", model);
	//m_WaterPlane->Render();
#endif

	if (Renderer::debugAxis)
		m_Axis->Render(m_Camera);

	RenderEnd();
}

#if 0 OLD_HEIGHTMAP_GENERATION_CODE
void GameLayer::GenerateHeightMap()
{
#pragma warning ( push )
#pragma warning ( disable: 6386)
	unsigned char* bytes = new unsigned char[m_HeightMap->GetWidth() * m_HeightMap->GetHeight() * 4];
	for (uint32_t y = 0; y < m_HeightMap->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_HeightMap->GetWidth(); x++)
		{
			uint32_t i = y * m_HeightMap->GetWidth() + x;
			const double noise = perlin.octave2D_01(x * frequency, y * frequency, octaves);
			const uint32_t height = (uint32_t)(std::clamp(noise, 0.01, 0.99) * 255);

			bytes[4 * i + 0] = height;
			bytes[4 * i + 1] = height;
			bytes[4 * i + 2] = height;
			bytes[4 * i + 3] = 255;
		}
	}
	m_HeightMap->LoadData(bytes, GL_RGBA, GL_RGBA);
	delete[] bytes;
#pragma warning (pop)
}
#endif

void GameLayer::OnImGuiRender(float dt)
{
	ImGui::Begin("Info");
	ImGui::DragFloat3("Camera position", &m_Camera->GetPosition()[0], 0.01f, -500.0f, 500.0f);
	ImGui::DragFloat3("Camera orientation", &m_Camera->GetOrientation()[0], 0.01f, -0.99f, 0.99f);
	static uint32_t FPS = 0u;
	static float lastFPSUpdate = 0.0f;
	static float sumDt = 0.0f;
	static uint32_t numFrames = 0u;
	lastFPSUpdate += dt;
	sumDt += dt;
	numFrames++;
	if (lastFPSUpdate >= 0.5f) {
		lastFPSUpdate = 0.0f;
		FPS = (float)(numFrames) / sumDt;
		numFrames = 0u;
		sumDt = 0.0f;
	}
	ImGui::Text("FPS: %d", FPS);
	ImGui::End();
	
	ImGui::Begin("Controls");
	ImGui::Text("Move - WASD");
	ImGui::Text("Look around - Mouse");
	ImGui::Text("Move up - Space");
	ImGui::Text("Move down - C");
	ImGui::Text("Fast - Shift");
	ImGui::Text("Slow - Ctrl");
	ImGui::Text("Debug axis - F3");
	ImGui::Text("Wireframe - F");
	ImGui::Text("Cursor - Tab");
	ImGui::End();
	
	ImGui::Begin("Noise props");
	ImGui::SliderFloat("Amlitude", &m_Amplitude, 0.01f, 1.0f);
	ImGui::SliderFloat("Frequency", &m_Frequency, 0.01f, 5.0f);
	ImGui::SliderFloat("Gain", &m_Gain, 0.01f, 0.5f);
	ImGui::SliderFloat("Scale", &m_Scale, 0.001f, 0.3f);
	ImGui::SliderFloat("HeightOffset", &m_HeightOffset, 0.0f, 100.0f);
	ImGui::SliderFloat2("NoiseOffset", &m_NoiseOffset[0], 0.0f, 10.0f);
	ImGui::End();

	ImGui::Begin("Landscape");
	if (ImGui::Button("Generate")) this->GenerateTerrain();
	ImGui::SliderFloat("MaxHeight", &m_MaxHeight, 0.0f, 1000.f);
	ImGui::SliderFloat("FogGradient", &m_FogGradient, 0.0f, 5.f);
	ImGui::SliderFloat("FogDensity", &m_FogDensity, 0.0f, 0.01f);
	ImGui::Checkbox("Normals", &m_TerrainNormals);
	ImGui::End();
	
	ImGui::Begin("Water");
	ImGui::SliderFloat("Level", &m_WaterLevel, -50.0f, 100.0f);
	ImGui::Checkbox("Normals", &m_WaterNormals);
	ImGui::SliderFloat("A - Steepness", &m_WaveA[2], 0.0f, 1.0f);
	ImGui::SliderFloat("A - Wavelength", &m_WaveA[3], 10.0f, 75.0f);
	ImGui::SliderFloat2("A - Direction", &m_WaveA[0], -1.0f, 1.0f);
	ImGui::SliderFloat("B - Steepness", &m_WaveB[2], 0.0f, 1.0f);
	ImGui::SliderFloat("B - Wavelength", &m_WaveB[3], 10.0f, 75.0f);
	ImGui::SliderFloat2("B - Direction", &m_WaveB[0], -1.0f, 1.0f);
	ImGui::SliderFloat("C - Steepness", &m_WaveC[2], 0.0f, 1.0f);
	ImGui::SliderFloat("C - Wavelength", &m_WaveC[3], 10.0f, 75.0f);
	ImGui::SliderFloat2("C - Direction", &m_WaveC[0], -1.0f, 1.0f);
	ImGui::End();

	ImGui::Begin("Viewport");
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	float my_tex_w = viewportPanelSize.x;
	float my_tex_h = viewportPanelSize.y;
	if (m_ViewportSize.x != viewportPanelSize.x || m_ViewportSize.y != viewportPanelSize.y)
	{
		m_FrameBuffer->Resize(my_tex_w, my_tex_h);
		m_Camera->Resize(my_tex_w, my_tex_h);
		m_ViewportSize.x = viewportPanelSize.x;
		m_ViewportSize.y = viewportPanelSize.y;
	}
	else
	{
		ImVec2 uv_min = ImVec2(0.0f, 1.0f); // Top-left
		ImVec2 uv_max = ImVec2(1.0f, 0.0f); // Lower-right
		ImGui::Image((ImTextureID)m_FrameBuffer->GetTextureId(), ImVec2(my_tex_w, my_tex_h), uv_min, uv_max);
	}
	ImGui::End();

	ImGui::Begin("Viewport info");
	ImGui::Text("Width: %.0fpx", m_ViewportSize.x);
	ImGui::Text("Height: %.0fpx", m_ViewportSize.y);
	ImGui::End();

	ImGui::Begin("Vendor info");
	static IDXGIFactory4* pFactory{};
	CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory);
	static IDXGIAdapter3* adapter{};
	pFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(&adapter));
	static DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo;
	adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);
	const size_t usedVRAM = videoMemoryInfo.CurrentUsage / 1024 / 1024;
	const size_t maxVRAM = videoMemoryInfo.Budget / 1024 / 1024;
	ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
	ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
	ImGui::Text("Version: %s", glGetString(GL_VERSION));
	int versionMajor, versionMinor;
	glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
	glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
	ImGui::Text("OpenGL %d.%d", versionMajor, versionMinor);
	ImGui::Text("VRAM: %d/%d MB", usedVRAM, maxVRAM);
	ImGui::End();
}

void GameLayer::OnScreenshot()
{
	printf("Taking screenshot\n");
	const size_t dataSize = m_ViewportSize.y * m_ViewportSize.x * 3;
	uint32_t* data = new uint32_t[dataSize];
	glGetTextureImage(m_FrameBuffer->GetTextureId(), 0, GL_RGB, GL_UNSIGNED_INT, dataSize * 4, data);
	stbi_write_png("stbpng.png", m_ViewportSize.x, m_ViewportSize.y, 3, data, m_ViewportSize.x * 3);
	delete[] data;
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

void GameLayer::GenerateTerrain()
{
	m_ComputeShader->Use();
	m_ComputeShader->SetUniform("u_Amplitude", m_Amplitude);
	m_ComputeShader->SetUniform("u_Gain", m_Gain);
	m_ComputeShader->SetUniform("u_Frequency", m_Frequency);
	m_ComputeShader->SetUniform("u_Scale", m_Scale);
	m_ComputeShader->SetUniform("u_NoiseOffset", m_NoiseOffset);

	for (int z = -1; z <= 1; z++)
	{
		for (int x = -1; x <= 1; x++)
		{
			int index = (z + 1) * 3 + (x + 1);
			m_ComputeShader->SetUniform("u_WorldOffset", glm::vec2(x * (int)planeSize, z * (int)planeSize));
			m_HeightMaps[index]->BindImage();
			m_ComputeShader->Dispatch(glm::uvec3(ceil(planeSize / 8), ceil(planeSize / 4), 1));
		}
	}
}

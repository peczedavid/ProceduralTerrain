// TODO: - offset terrain height relative to amplitude so it starts at 0
//		 - fix seames between chunks (maybe (planeSize+1)*(planeSize+1) heightmaps)
//		 - make renderer into static class
//		 - make FPSPool into own class/struct

#include "Sandbox/GameLayer.h"
#include "Core/Application.h"
#include <glad/glad.h>
#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include <iostream>
#include <random>
#include "Rendering/Renderer.h"
#include <stb_image_write.h>
#include <stb_image.h>
#include <ctime>
#include <sstream>
#include "Sandbox/GameLayerImGui.h"

constexpr uint32_t planeSize = 1024u;
constexpr uint32_t planeDivision = 25u;
constexpr uint32_t waterPlaneSize = 1000u;
constexpr uint32_t waterPlaneDivision = 100u;

constexpr uint32_t FFTResoltion = 256u;

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

	GenerateTerrain();

	m_H0k = new Texture2D(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_H0minusk = new Texture2D(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_H0ComputeShader = new ComputeShader("src/Rendering/Shaders/glsl/water-fft/h0.comp");

	GenerateH0Textures();

	m_UI = new GameLayerImGui(this);
}

void GameLayer::OnUpdate(float dt)
{
	RenderStart();

	UpdateFPS(dt);

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

	static glm::mat4 model;

	m_TerrainShader->Use();
	m_GroundTexture->Bind(1);
	m_RockTexture->Bind(2);
	m_TerrainShader->SetUniform("u_MaxLevel", m_MaxHeight);
	m_TerrainShader->SetUniform("u_View", m_Camera->GetView());
	m_TerrainShader->SetUniform("u_FogGradient", m_FogGradient);
	m_TerrainShader->SetUniform("u_FogDensity", m_FogDensity);
	m_TerrainShader->SetUniform("u_NormalView", m_TerrainNormals ? 1 : 0);

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

#if 0
	m_WaterShader->Use();
	m_WaterTexture->Bind(0);
	m_WaterShader->SetUniform("u_ViewProj", m_Camera->GetMatrix());
	m_WaterShader->SetUniform("u_View", m_Camera->GetView());
	m_WaterShader->SetUniform("u_FogGradient", m_FogGradient);
	m_WaterShader->SetUniform("u_FogDensity", m_FogDensity);
	m_WaterShader->SetUniform("u_WaveA", m_WaveA);
	m_WaterShader->SetUniform("u_WaveB", m_WaveB);
	m_WaterShader->SetUniform("u_WaveC", m_WaveC);
	m_WaterShader->SetUniform("u_Time", t);
	m_WaterShader->SetUniform("u_NormalView", m_WaterNormals ? 1 : 0);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)waterPlaneSize / 2.f, m_WaterLevel, -(float)waterPlaneSize / 2.f));
	m_WaterShader->SetUniform("u_Model", model);
	m_WaterPlane->Render();
#endif

	if (Renderer::debugView)
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
	m_UI->ContolsPanel();
	m_UI->NoisePanel();
	m_UI->LandscapePanel();
	m_UI->WaterPanel();
	m_UI->ViewportPanel();
	m_UI->TexturesPanel();
	m_UI->FPSGraphPanel();
	m_UI->VendorInfoPanel();
	m_UI->FFTPanel();
	m_UI->GraphicsSettingsPanel();
	if (Renderer::debugView) m_UI->DebugOverlayPanel();
}

void GameLayer::OnScreenshot()
{
	uint8_t* data = new uint8_t[m_ViewportSize.y * m_ViewportSize.x * 3];
	m_FrameBuffer->Bind();
	glPixelStorei(GL_PACK_ALIGNMENT, 1); // OpenGL memory to RAM, 1 byte per color channel
	glReadPixels(0, 0, m_ViewportSize.x, m_ViewportSize.y, GL_RGB, GL_UNSIGNED_BYTE, data);
	FrameBuffer::Default();
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);
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
			m_ComputeShader->Dispatch(glm::uvec3(ceil(planeSize / 16), ceil(planeSize / 16), 1));
		}
	}
}

void GameLayer::UpdateFPS(float dt)
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
		Renderer::AddFPSSample(static_cast<float>(m_FPS));
		lastFPSSampleUpdate = 0;
	}

	if (lastFPSUpdate >= 0.5f) {
		lastFPSUpdate = 0.0f;
		m_FPS = (float)(numFrames) / sumDt;
		numFrames = 0u;
		sumDt = 0.0f;
		auto result = std::max_element(Renderer::fpsPool.begin(), Renderer::fpsPool.end());
		Renderer::maxFPS = Renderer::fpsPool.at(std::distance(Renderer::fpsPool.begin(), result));
	}
}

void GameLayer::GenerateH0Textures()
{
	m_H0k->BindImage(0);
	m_H0minusk->BindImage(1);
	m_H0ComputeShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));
}
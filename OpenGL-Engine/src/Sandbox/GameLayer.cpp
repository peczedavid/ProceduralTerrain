// TODO: - offset terrain height relative to amplitude so it starts at 0
//		 - fix seames between chunks (maybe (planeSize+1)*(planeSize+1) heightmaps)
// 
//		 - make renderer into static class			DONE
//		 - make FPSPool into own class/struct		DONE
//		 - put shaders into assets					DONE
//       - dist post build command					DONE
//		   (copy assets and imgui.ini)
//
//		 - uniforms in map							DONE
//		 - hot reaload shaders						DONE
// 		 - single shader class						DONE
// 		 - shader library
//		 - precompiled header
//		 - Core.h macros for logging, assert,...
//		 - logging
//		 - reference
//		 - uniform buffer objects eg: camera uniform
// 
//		 - scene system (switching between scenes)
//		 - profiling (maybe benchmark scene)
//
//       - object loading
//		 - grass and tree rendering
//		 - trackball camera controls
// 
// Create single shader class, vector of shader src-s, { type, src }
// Compile based on type and src
// Parse for samplers, if has any, activate with tex-unit

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

GameLayer::GameLayer()
{
	m_TerrainShader = new Shader({
		"assets/GLSL/terrain/terrain.vert",
		"assets/GLSL/terrain/terrain.tesc",
		"assets/GLSL/terrain/terrain.tese",
		"assets/GLSL/terrain/terrain.frag"
		});
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	m_WaterShader = new Shader({
		"assets/GLSL/water/water.vert",
		"assets/GLSL/water/water.tesc",
		"assets/GLSL/water/water.tese",
		"assets/GLSL/water/water.frag"
		});
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	m_GroundPlane = new Plane(planeSize, planeDivision);
	m_WaterPlane = new Plane(waterPlaneSize, waterPlaneDivision);

	m_Camera = new Camera(glm::vec3(0, 64, 0), glm::vec3(0, -0.45f, -1.0f));
	m_Camera->Resize(1, 1);

	Shader* skyboxShader = new Shader({ "assets/GLSL/skybox.vert", "assets/GLSL/skybox.frag" });
	m_Skybox = new Skybox(skyboxShader);

	m_Axis = new Axis();

	m_FullscreenQuad = new FullscreenQuad();

	m_GroundTexture = new Texture2D("assets/Textures/ground-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_RockTexture = new Texture2D("assets/Textures/rock-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_WaterTexture = new Texture2D("assets/Textures/water-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	m_FrameBuffer = new FrameBuffer(1, 1);
	m_PostProcessShader = new Shader({
		"assets/GLSL/postprocess.vert",
		"assets/GLSL/postprocess.frag"
		});
	FrameBuffer::Default();

	m_TerrainComputeShader = new Shader({ "assets/GLSL/noise.comp" });
	m_TerrainComputeShader->Use();
	m_TerrainComputeShader->SetUniform("u_Amplitude", m_Amplitude);
	m_TerrainComputeShader->SetUniform("u_Gain", m_Gain);
	m_TerrainComputeShader->SetUniform("u_Frequency", m_Frequency);
	m_TerrainComputeShader->SetUniform("u_Scale", m_Scale);
	m_TerrainComputeShader->SetUniform("u_NoiseOffset", m_NoiseOffset);

	for (int z = -1; z <= 1; z++)
		for (int x = -1; x <= 1; x++)
			m_HeightMaps.push_back(new Texture2D(planeSize, planeSize, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGBA32F));

	GenerateTerrain();

	m_H0k = new Texture2D(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_H0minusk = new Texture2D(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_HtDy = new Texture2D(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_HtDx = new Texture2D(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_HtDz = new Texture2D(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_H0ComputeShader = new Shader({ "assets/GLSL/water-fft/h0.comp" });
	m_HktComputeShader = new Shader({ "assets/GLSL/water-fft/hkt.comp" });
	m_TwiddleTexture = new Texture2D(8, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_TwiddleShader = new Shader({ "assets/GLSL/water-fft/twiddle.comp" });
	m_PingPong0 = new Texture2D(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_PingPong1 = new Texture2D(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_Displacement = new Texture2D(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	m_ButterflyShader = new Shader({ "assets/GLSL/water-fft/butterfly.comp" });
	m_CopyShader = new Shader({ "assets/GLSL/water-fft/copy.comp" });
	m_InversionShader = new Shader({ "assets/GLSL/water-fft/inversion.comp" });

	GenerateFFTTextures();

	m_UI = new GameLayerImGui(this);
}

void GameLayer::OnUpdate(float dt)
{
	RenderStart();

	UpdateFPS(dt);

	m_Time += dt;
	float fov = 45.0f, nearPlane = 0.1f, farPlane = 3000.0f;

	m_Camera->UpdateMatrix(fov, nearPlane, farPlane);
	if (!Application::Get().IsCursor())
		m_Camera->Update(dt);

	m_TerrainShader->Use();
	m_TerrainShader->SetUniform("u_ViewProj", m_Camera->GetMatrix());

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

	if (Renderer::DebugView)
		m_Axis->Render(m_Camera);

	FFTLoop();

	RenderEnd();
}

void GameLayer::OnImGuiRender(float dt)
{
	m_UI->ContolsPanel();
	m_UI->NoisePanel();
	m_UI->LandscapePanel();
	m_UI->WaterPanel();
	m_UI->ViewportPanel();
	m_UI->TexturesPanel();
	m_UI->VendorInfoPanel();
	m_UI->FFTPanel();
	m_UI->GraphicsSettingsPanel();
	m_UI->ShadersPanel();
	if (Renderer::DebugView) m_UI->DebugOverlayPanel();
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
	m_TerrainComputeShader->Use();
	m_TerrainComputeShader->SetUniform("u_Amplitude", m_Amplitude);
	m_TerrainComputeShader->SetUniform("u_Gain", m_Gain);
	m_TerrainComputeShader->SetUniform("u_Frequency", m_Frequency);
	m_TerrainComputeShader->SetUniform("u_Scale", m_Scale);
	m_TerrainComputeShader->SetUniform("u_NoiseOffset", m_NoiseOffset);

	for (int z = -1; z <= 1; z++)
	{
		for (int x = -1; x <= 1; x++)
		{
			int index = (z + 1) * 3 + (x + 1);
			m_TerrainComputeShader->SetUniform("u_WorldOffset", glm::vec2(x * (int)planeSize, z * (int)planeSize));
			m_HeightMaps[index]->BindImage();
			m_TerrainComputeShader->Dispatch(glm::uvec3(ceil(planeSize / 16), ceil(planeSize / 16), 1));
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

void GameLayer::GenerateFFTTextures()
{
	m_H0ComputeShader->Use();
	m_H0k->BindImage(0);
	m_H0minusk->BindImage(1);
	m_H0ComputeShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));

	m_TwiddleShader->Use();
	m_TwiddleTexture->BindImage(0);
	m_TwiddleShader->Dispatch(glm::uvec3(8, ceil(FFTResoltion / 16), 1));
}

void GameLayer::FFTLoop()
{
	m_HktComputeShader->Use();
	m_HtDy->BindImage(0);
	m_HtDx->BindImage(1);
	m_HtDz->BindImage(2);
	m_H0k->BindImage(3);
	m_H0minusk->BindImage(4);
	m_HktComputeShader->SetUniform("u_Time", m_Time);
	m_HktComputeShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));

	m_CopyShader->Use();
	m_HtDy->BindImage(0);
	m_PingPong0->BindImage(1);
	m_CopyShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));
	m_ButterflyShader->Use();
	m_TwiddleTexture->BindImage(0);
	m_PingPong0->BindImage(1);
	m_PingPong1->BindImage(2);

	int pingpong = 0;

	m_ButterflyShader->SetUniform("u_Direction", 0);
	for (int i = 0; i < log2(FFTResoltion); i++)
	{
		m_ButterflyShader->SetUniform("u_Stage", i);
		m_ButterflyShader->SetUniform("u_PingPong", pingpong);
		m_ButterflyShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));
		pingpong++;
		pingpong = pingpong % 2;
	}

	m_ButterflyShader->SetUniform("u_Direction", 1);
	for (int i = 0; i < log2(FFTResoltion); i++)
	{
		m_ButterflyShader->SetUniform("u_Stage", i);
		m_ButterflyShader->SetUniform("u_PingPong", pingpong);
		m_ButterflyShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));
		pingpong++;
		pingpong = pingpong % 2;
	}

	m_InversionShader->Use();
	m_Displacement->BindImage(0);
	m_PingPong0->BindImage(1);
	m_PingPong1->BindImage(2);
	m_InversionShader->SetUniform("u_PingPong", pingpong);
	m_InversionShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));
}

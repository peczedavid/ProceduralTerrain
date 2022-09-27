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
// 		 - shader library							DONE
//		 - reference								DONE
// 
//		 - precompiled header						DONE
//		 - logging									DONE
//		 - macros for logging, assert				DONE
//		 - uniform buffer objects eg camera uniform DONE
// 
//		 - nice gerstner waves
// 
//		 - scene system (switching between scenes)
//		 - infinite grid CTRL+G toggle
//		 - materials, pbr rendering?
//       - object loading
//		 - tree rendering
// 
//		 - screenshots in dist mode
//		 - profiling (maybe benchmark scene)
//		 - trackball camera controls

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

	m_GroundPlane = CreateRef<Plane>(planeSize, planeDivision);
	m_WaterPlane = CreateRef<Plane>(waterPlaneSize, waterPlaneDivision);

	m_Camera = CreateRef<Camera>(glm::vec3(0, 64, 0), glm::vec3(0, -0.45f, -1.0f));
	m_Camera->Resize(1, 1);

	auto skyboxShader = CreateShaderRef("assets/GLSL/skybox.vert", "assets/GLSL/skybox.frag");
	m_ShaderLibrary.Add("Skybox shader", skyboxShader);
	m_Skybox = CreateRef<Skybox>(skyboxShader);

	auto axisShader = CreateShaderRef("assets/GLSL/axis/axis.vert", "assets/GLSL/axis/axis.frag");
	m_ShaderLibrary.Add("Axis shader", axisShader);
	m_Axis = CreateRef<Axis>(axisShader);

	m_FullscreenQuad = CreateRef<FullscreenQuad>();

	m_GroundTexture = CreateRef<Texture2D>("assets/Textures/ground-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_RockTexture = CreateRef<Texture2D>("assets/Textures/rock-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_WaterTexture = CreateRef<Texture2D>("assets/Textures/water-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

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
	
	//m_ShaderLibrary.Add("H0 compute shader", CreateShaderRef("assets/GLSL/water-fft/h0.comp"));
	//m_ShaderLibrary.Add("Hkt compute shader", CreateShaderRef("assets/GLSL/water-fft/hkt.comp"));
	//m_ShaderLibrary.Add("Twiddle shader", CreateShaderRef("assets/GLSL/water-fft/twiddle.comp"));
	//m_ShaderLibrary.Add("Butterfly shader", CreateShaderRef("assets/GLSL/water-fft/butterfly.comp"));
	//m_ShaderLibrary.Add("Copy compute shader", CreateShaderRef("assets/GLSL/water-fft/copy.comp"));
	//m_ShaderLibrary.Add("Inversion shader", CreateShaderRef("assets/GLSL/water-fft/inversion.comp"));
	//m_H0k = CreateRef<Texture2D>(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	//m_H0minusk = CreateRef<Texture2D>(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	//m_HtDy = CreateRef<Texture2D>(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	//m_HtDx = CreateRef<Texture2D>(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	//m_HtDz = CreateRef<Texture2D>(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	//m_TwiddleTexture = CreateRef<Texture2D>(8, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	//m_PingPong0 = CreateRef<Texture2D>(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	//m_PingPong1 = CreateRef<Texture2D>(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);
	//m_Displacement = CreateRef<Texture2D>(FFTResoltion, FFTResoltion, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA32F);

	//GenerateFFTTextures();

	m_UI = CreateScope<GameLayerImGui>(this);

	glGenBuffers(1, &m_CameraUBO);
	const size_t cameraUBOsize = 3 * sizeof(glm::mat4);
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
}

void GameLayer::OnUpdate(const float dt)
{
	RenderStart();

	UpdateFPS(dt);

	m_Time += dt;
	const float fov = 45.0f, const nearPlane = 0.1f, const farPlane = 3000.0f;

	m_Camera->UpdateMatrix(fov, nearPlane, farPlane);
	if (!Application::Get().IsCursor())
		m_Camera->Update(dt);

	glBindBuffer(GL_UNIFORM_BUFFER, m_CameraUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0 * sizeof(glm::mat4), sizeof(glm::mat4), &m_Camera->GetView()[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 1 * sizeof(glm::mat4), sizeof(glm::mat4), &m_Camera->GetProj()[0][0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), &m_Camera->GetMatrix()[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_Skybox->Render(m_Camera);

	glm::mat4 model;

	auto terrainShader = m_ShaderLibrary.Get("Terrain shader");
	terrainShader->Use();
	m_GroundTexture->Bind(1);
	m_RockTexture->Bind(2);
	terrainShader->SetUniform("u_MaxLevel", m_MaxHeight);
	terrainShader->SetUniform("u_FogGradient", m_FogGradient);
	terrainShader->SetUniform("u_FogDensity", m_FogDensity);
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

#if 1
	glDepthMask(GL_FALSE);
	auto waterShader = m_ShaderLibrary.Get("Water shader");
	waterShader->Use();
	m_WaterTexture->Bind(0);
	waterShader->SetUniform("u_FogGradient", m_FogGradient);
	waterShader->SetUniform("u_FogDensity", m_FogDensity);
	waterShader->SetUniform("u_Time", m_Time);
	waterShader->SetUniform("u_Shininess", m_WaterShininess);
	waterShader->SetUniform("u_Reflectivity", m_WaterReflectivity);
	waterShader->SetUniform("u_CameraPos", m_Camera->GetPosition());
	waterShader->SetUniform("u_NormalView", m_WaterNormals ? 1 : 0);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)waterPlaneSize / 2.f, m_WaterLevel, -(float)waterPlaneSize / 2.f));
	waterShader->SetUniform("u_Model", model);
	m_WaterPlane->Render();
	glDepthMask(GL_TRUE);
#endif

	if (Renderer::DebugView)
		m_Axis->Render(m_Camera);

	//FFTLoop();

	RenderEnd();
}

void GameLayer::OnImGuiRender(const float dt)
{
	m_UI->ContolsPanel();
	m_UI->NoisePanel();
	m_UI->LandscapePanel();
	m_UI->WaterPanel();
	m_UI->ViewportPanel();
	m_UI->TexturesPanel();
	m_UI->VendorInfoPanel();
	//m_UI->FFTPanel();
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
			terrainComputeShader->SetUniform("u_WorldOffset", glm::vec2(x * (int)planeSize, z * (int)planeSize));
			m_HeightMaps[index]->BindImage();
			terrainComputeShader->Dispatch(glm::uvec3(ceil(planeSize / 16), ceil(planeSize / 16), 1));
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

void GameLayer::GenerateFFTTextures()
{
	auto h0computeShader = m_ShaderLibrary.Get("H0 compute shader");
	h0computeShader->Use();
	m_H0k->BindImage(0);
	m_H0minusk->BindImage(1);
	h0computeShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));

	auto twiddleShader = m_ShaderLibrary.Get("Twiddle shader");
	twiddleShader->Use();
	m_TwiddleTexture->BindImage(0);
	twiddleShader->Dispatch(glm::uvec3(8, ceil(FFTResoltion / 16), 1));
}

void GameLayer::FFTLoop()
{
	auto hktComputeShader = m_ShaderLibrary.Get("Hkt compute shader");
	hktComputeShader->Use();
	m_HtDy->BindImage(0);
	m_HtDx->BindImage(1);
	m_HtDz->BindImage(2);
	m_H0k->BindImage(3);
	m_H0minusk->BindImage(4);
	hktComputeShader->SetUniform("u_Time", m_Time);
	hktComputeShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));

	auto copyShader = m_ShaderLibrary.Get("Copy compute shader");
	auto butterflyShader = m_ShaderLibrary.Get("Butterfly shader");
	copyShader->Use();
	m_HtDy->BindImage(0);
	m_PingPong0->BindImage(1);
	copyShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));
	butterflyShader->Use();
	m_TwiddleTexture->BindImage(0);
	m_PingPong0->BindImage(1);
	m_PingPong1->BindImage(2);

	int pingpong = 0;

	butterflyShader->SetUniform("u_Direction", 0);
	for (int i = 0; i < log2(FFTResoltion); i++)
	{
		butterflyShader->SetUniform("u_Stage", i);
		butterflyShader->SetUniform("u_PingPong", pingpong);
		butterflyShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));
		pingpong++;
		pingpong = pingpong % 2;
	}

	butterflyShader->SetUniform("u_Direction", 1);
	for (int i = 0; i < log2(FFTResoltion); i++)
	{
		butterflyShader->SetUniform("u_Stage", i);
		butterflyShader->SetUniform("u_PingPong", pingpong);
		butterflyShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));
		pingpong++;
		pingpong = pingpong % 2;
	}

	auto inversionShader = m_ShaderLibrary.Get("Inversion shader");
	inversionShader->Use();
	m_Displacement->BindImage(0);
	m_PingPong0->BindImage(1);
	m_PingPong1->BindImage(2);
	inversionShader->SetUniform("u_PingPong", pingpong);
	inversionShader->Dispatch(glm::uvec3(ceil(FFTResoltion / 16), ceil(FFTResoltion / 16), 1));
}
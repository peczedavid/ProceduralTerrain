// TODO: - offset terrain height relative to amplitude so it starts at 0
//		 - fix seames between chunks (maybe (planeSize+1)*(planeSize+1) heightmaps)
//		 - axis, fix length

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
#include <stb_image.h>
#include <ctime>
#include <sstream>

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

void DrawImage(uint32_t textureId);

void GameLayer::OnImGuiRender(float dt)
{
	if (ImGui::Begin("Controls"))
	{
		ImGui::Text("WASD - Move");
		ImGui::Text("Mouse - Look around");
		ImGui::Text("Space - Move up");
		ImGui::Text("C - Move down");
		ImGui::Text("Shift - Fast");
		ImGui::Text("Ctrl - Slow");
		ImGui::Text("F2 - Screenshot");
		ImGui::Text("F3 - Debug info");
		ImGui::Text("F - Wireframe");
		ImGui::Text("Tab - Cursor");
		ImGui::Text("Esc - Close");
	}
	ImGui::End();

	if (ImGui::Begin("Noise props"))
	{
		ImGui::SliderFloat("Amlitude", &m_Amplitude, 0.01f, 1.0f);
		ImGui::SliderFloat("Frequency", &m_Frequency, 0.01f, 5.0f);
		ImGui::SliderFloat("Gain", &m_Gain, 0.01f, 0.5f);
		ImGui::SliderFloat("Scale", &m_Scale, 0.001f, 0.3f);
		ImGui::SliderFloat2("NoiseOffset", &m_NoiseOffset[0], 0.0f, 10.0f);
	}
	ImGui::End();

	if (ImGui::Begin("Landscape"))
	{
		if (ImGui::Button("Generate")) this->GenerateTerrain();
		ImGui::SliderFloat("MaxHeight", &m_MaxHeight, 0.0f, 1000.f);
		ImGui::SliderFloat("FogGradient", &m_FogGradient, 0.0f, 5.f);
		ImGui::SliderFloat("FogDensity", &m_FogDensity, 0.0f, 0.01f);
		ImGui::Checkbox("Normals", &m_TerrainNormals);
	}
	ImGui::End();

	if (ImGui::Begin("Water"))
	{
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
	}
	ImGui::End();

	if (ImGui::Begin("Viewport"))
	{
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
	}
	ImGui::End();

	if (ImGui::Begin("Textures"))
	{
		for (int i = 2; i >= 0; i--)
		{
			for (int j = 0; j < 3; j++)
			{
				DrawImage(m_HeightMaps[i * 3 + j]->GetId());
				if (j < 2)
					ImGui::SameLine();
			}
		}
	}
	ImGui::End();

	if (ImGui::Begin("Waves"))
	{
		constexpr int size = 500;
		static float values[size] = { };
		for (int i = 0; i < size; i++)
			values[i] = sinf(i / 10.0f);
		ImGui::PlotLines("Sin", values, size, 0, 0, -1.0f, 1.0f, ImVec2(0.0, 80.0f));
	}
	ImGui::End();

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
	if (Renderer::debugView)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("Debug overaly", &Renderer::debugView, window_flags);
		ImGui::Text("Viewport size: %.0fx%.0f", m_ViewportSize.x, m_ViewportSize.y);
		const glm::vec3 cameraPos = m_Camera->GetPosition();
		ImGui::Text("XYZ: %.2f / %.2f / %.2f", cameraPos.x, cameraPos.y, cameraPos.y);
		const glm::vec3 cameraOri = m_Camera->GetOrientation();
		ImGui::Text("Facing: %.2f / %.2f / %.2f", cameraOri.x, cameraOri.y, cameraOri.y);
		ImGui::Text("%d FPS", FPS);
		ImGui::End();
		ImGui::PopStyleVar();
	}

	static bool firstRun = true;
	static IDXGIFactory4* pFactory{};
	static IDXGIAdapter3* adapter{};
	static DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo{};
	if (firstRun)
	{
		CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory);
		pFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(&adapter));
		firstRun = false;
	}
	if (ImGui::Begin("Vendor info"))
	{
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
	}
	ImGui::End();
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
			m_ComputeShader->Dispatch(glm::uvec3(ceil(planeSize / 8), ceil(planeSize / 4), 1));
		}
	}
}

void DrawImage(uint32_t textureId)
{
	ImGuiIO& io = ImGui::GetIO();
	ImTextureID my_tex_id = (ImTextureID)textureId;
	float my_tex_w = 200.0f;
	float my_tex_h = 200.0f;
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 uv_min = ImVec2(0.0f, 1.0f); // Top-left
		ImVec2 uv_max = ImVec2(1.0f, 0.0f); // Lower-right
		ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			float region_sz = 32.0f;
			float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
			float region_y = io.MousePos.y - pos.y;
			region_y = my_tex_h - region_y;
			region_y = region_y - region_sz * 0.5f;
			float zoom = 7.0f;
			if (region_x < 0.0f) { region_x = 0.0f; }
			else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
			if (region_y < 0.0f) { region_y = 0.0f; }
			else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }
			ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
			ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
			ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y + region_sz) / my_tex_h);
			ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y) / my_tex_h);
			ImGui::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1);
			ImGui::EndTooltip();
		}
	}
}

/*
std::vector<int>& FFT(std::vector<int>& P)
{
	const size_t n = P.size();
	if (n == 1)
		return P;

	float w = powf(E, (2.0f * PI) / n);
	std::vector<int> Pe, Po;
	for (size_t i = 0; i < n; i++)
	{
		if (i % 2 == 0)
			Pe.push_back(P[i]);
		else
			Po.push_back(P[i]);
	}
	std::vector<int> ye, yo;
	ye = FFT(Pe);
	yo = FFT(Po);

	std::vector<int> y;
	for (size_t i = 0; i < n; i++)
		y.push_back(0);

	for (size_t j = 0; j < (n / 2); j++)
	{
		y[j] = ye[j] + powf(w, j) * yo[j];
		y[j + n / 2] = ye[j] - powf(w, j) * yo[j];
	}

	return y;
}
*/
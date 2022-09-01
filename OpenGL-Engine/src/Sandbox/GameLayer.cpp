#include "Sandbox/GameLayer.h"
#include "Core/Application.h"
#include <glad/glad.h>
#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include <iostream>
#include <random>
#include "Rendering/Renderer.h"

constexpr uint32_t heighMapSize = 2048u;
constexpr uint32_t planeSize = 1000u;
constexpr uint32_t planeDivision = 20u;
constexpr uint32_t waterPlaneSize = 500u;
constexpr uint32_t waterPlaneDivision = 25u;

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

	m_Plane = new Plane(planeSize, planeDivision);
	m_WaterPlane = new Plane(waterPlaneSize, waterPlaneDivision);

	m_Camera = new Camera(glm::vec3(0, 64, 0), glm::vec3(0, -0.45f, -1.0f));

	BasicShader* skyboxShader = new BasicShader("src/Rendering/Shaders/glsl/skybox.vert", "src/Rendering/Shaders/glsl/skybox.frag");
	m_Skybox = new Skybox(skyboxShader);

	m_Axis = new Axis();

	// ----------- CUBE GENEREATION START -----------
	glGenVertexArrays(1, &m_VaoCube);
	glGenBuffers(1, &m_VboCube);
	glGenBuffers(1, &m_EboCube);

	glBindVertexArray(m_VaoCube);
	glBindBuffer(GL_ARRAY_BUFFER, m_VboCube);

	float s = 0.5f;
	float vertices[24 * 5] = {
		// FAR FACE
		 s, -s, -s, 0, 0,   -s, -s, -s, 1, 0,   -s,  s, -s, 1, 1,    s,  s, -s, 0, 1,
		 // CLOSE FACE															
		 -s, -s,  s, 0, 0,    s, -s,  s, 1, 0,    s,  s,  s, 1, 1,   -s,  s,  s, 0, 1,
		 // RIGHT FACE															
		  s, -s,  s, 0, 0,    s, -s, -s, 1, 0,    s,  s, -s, 1, 1,    s,  s,  s, 0, 1,
		  // LEFT FACE															
		  -s, -s, -s, 0, 0,   -s, -s,  s, 1, 0,   -s,  s,  s, 1, 1,   -s,  s, -s, 0, 1,
		  // UP FACE																
		  -s,  s,  s, 0, 0,    s,  s,  s, 1, 0,    s,  s, -s, 1, 1,   -s,  s, -s, 0, 1,
		  // DOWN FACE															
		  -s, -s, -s, 0, 0,    s, -s, -s, 1, 0,    s, -s,  s, 1, 1,   -s, -s,  s, 0, 1,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EboCube);
	uint32_t indices[36] = {
		0,   1,  2,    0,  2,  3,
		4,   5,  6,    4,  6,  7,
		8,   9, 10,    8, 10, 11,
		12, 13, 14,   12, 14, 15,
		16, 17, 18,   16, 18, 19,
		20, 21, 22,   20, 22, 23
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// ----------- CUBE GENEREATION END -----------

	m_UvTexture = new Texture2D("assets/Textures/uv-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_GroundTexture = new Texture2D("assets/Textures/ground-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_RockTexture = new Texture2D("assets/Textures/rock-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_SnowTexture = new Texture2D("assets/Textures/snow-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_WaterTexture = new Texture2D("assets/Textures/water-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
}

void GameLayer::OnUpdate(float dt)
{
	static float t = 0.0f; t += dt;

	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float asp = (float)Application::Get().GetWindow()->GetWidth() / (float)Application::Get().GetWindow()->GetHeight();
	float fov = 45.0f, nearPlane = 0.1f, farPlane = 3000.0f;

	m_Camera->UpdateMatrix(fov, asp, nearPlane, farPlane);
	if (!Application::Get().IsCursor())
		m_Camera->Update(dt);

	m_TerrainShader->Use();
	m_TerrainShader->SetUniform("u_ViewProj", m_Camera->GetMatrix());
	m_Shader->Use();
	m_Shader->SetUniform("u_ViewProj", m_Camera->GetMatrix());

	m_UvTexture->Bind();
	glBindVertexArray(m_VaoCube);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 3, -6));
	static float rot = 0.0f;
	rot += 0.65f * dt;
	model = glm::rotate(model, rot, glm::vec3(0.0f, 1.0f, 0.0f));
	static float scale = 1.0f;
	scale = sinf(t * 2.0f) * 0.4f + 1.0f;
	model = glm::scale(model, glm::vec3(scale, scale, scale));
	m_Shader->SetUniform("u_Model", model);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	m_Skybox->Render(m_Camera);

	m_TerrainShader->Use();
	m_GroundTexture->Bind(1);
	m_RockTexture->Bind(2);
	m_TerrainShader->SetUniform("u_MaxLevel", m_MaxHeight);
	m_TerrainShader->SetUniform("u_View", m_Camera->GetView());
	m_TerrainShader->SetUniform("u_Amplitude", m_Amplitude);
	m_TerrainShader->SetUniform("u_Gain", m_Gain);
	m_TerrainShader->SetUniform("u_Frequency", m_Frequency);
	m_TerrainShader->SetUniform("u_Scale", m_Scale);
	m_TerrainShader->SetUniform("u_HeightOffset", m_HeightOffset);
	m_TerrainShader->SetUniform("u_FogGradient", m_FogGradient);
	m_TerrainShader->SetUniform("u_FogDensity", m_FogDensity);
	m_TerrainShader->SetUniform("u_NoiseOffset", m_NoiseOffset);
	m_TerrainShader->SetUniform("u_NormalView", m_NormalView ? 1 : 0);
	int levelSize = 3;
	for (int z = -(levelSize-2); z < (levelSize - 1); z++)
	{
		for (int x = -(levelSize-2); x < (levelSize - 1); x++)
		{
			model = glm::translate(glm::mat4(1.0f), glm::vec3(x * (int)planeSize, 0, z * (int)planeSize));
			m_TerrainShader->SetUniform("u_Model", model);
			m_Plane->Render();
		}
	}

	m_WaterShader->Use();
	//m_WaterTexture->Bind(0);
	m_WaterShader->SetUniform("u_ViewProj", m_Camera->GetMatrix());
	m_WaterShader->SetUniform("u_View", m_Camera->GetView());
	m_WaterShader->SetUniform("u_FogGradient", m_FogGradient);
	m_WaterShader->SetUniform("u_FogDensity", m_FogDensity);
	m_WaterShader->SetUniform("u_Steepness", m_Steepness);
	m_WaterShader->SetUniform("u_WaveLength", m_WaveLength);
	m_WaterShader->SetUniform("u_Time", t);

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
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)waterPlaneSize/2.f, m_WaterLevel, -(float)waterPlaneSize /2.f));
	m_WaterShader->SetUniform("u_Model", model);
	m_WaterPlane->Render();
#endif
	

	if(Renderer::debugAxis)
		m_Axis->Render(m_Camera);
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
	ImGui::SliderFloat("MaxHeight", &m_MaxHeight, 0.0f, 1000.f);
	ImGui::SliderFloat("FogGradient", &m_FogGradient, 0.0f, 5.f);
	ImGui::SliderFloat("FogDensity", &m_FogDensity, 0.0f, 0.01f);
	ImGui::Checkbox("Normals", &m_NormalView);
	ImGui::End();

	ImGui::Begin("Water");
	ImGui::SliderFloat("Level", &m_WaterLevel, -50.0f, 50.0f);
	ImGui::SliderFloat("Steepness", &m_Steepness, 0.0f, 1.0f);
	ImGui::SliderFloat("Wavelength", &m_WaveLength, 10.0f, 75.0f);
	ImGui::End();
}

#include "Sandbox/GameLayer.h"
#include "Core/Application.h"
#include <glad/glad.h>
#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include <PerlinNoise.hpp>
#include <iostream>
#include <random>

constexpr uint32_t heighMapSize = 2048u;
constexpr uint32_t planeSize = 250u;
constexpr uint32_t planeDivision = 25u;

std::random_device rd; // obtain a random number from hardware
std::mt19937 gen(rd()); // seed the generator
std::uniform_int_distribution<> distr(10e3, 10e4); // define the range
siv::PerlinNoise::seed_type seed = distr(gen);
siv::PerlinNoise perlin{ seed };

float frequency = 0.002f;
int octaves = 4;

GameLayer::GameLayer()
{
	m_Shader = new Shader("src/Rendering/Shaders/glsl/default.vert", "src/Rendering/Shaders/glsl/default.frag");
	m_TessellationShader = new TessellationShader(
		"src/Rendering/Shaders/glsl/terrain.vert",
		"src/Rendering/Shaders/glsl/terrain.tesc",
		"src/Rendering/Shaders/glsl/terrain.tese",
		"src/Rendering/Shaders/glsl/terrain.frag");
	uint32_t textureUnit = glGetUniformLocation(m_TessellationShader->GetProgramId(), "u_Texture");
	glUniform1i(textureUnit, 0);
	textureUnit = glGetUniformLocation(m_TessellationShader->GetProgramId(), "u_GroundTexture");
	glUniform1i(textureUnit, 1);
	textureUnit = glGetUniformLocation(m_TessellationShader->GetProgramId(), "u_RockTexture");
	glUniform1i(textureUnit, 2);
	textureUnit = glGetUniformLocation(m_TessellationShader->GetProgramId(), "u_SnowTexture");
	glUniform1i(textureUnit, 3);

	m_HeightMap = new Texture2D(heighMapSize, heighMapSize, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_RGBA);
	GenerateHeightMap();

	m_Plane = new Plane(planeSize, planeDivision);

	m_Camera = new Camera(glm::vec3(0, 128, 256), glm::vec3(0, -0.45f, -1.0f));

	Shader* skyboxShader = new Shader("src/Rendering/Shaders/glsl/skybox.vert", "src/Rendering/Shaders/glsl/skybox.frag");
	m_Skybox = new Skybox(skyboxShader);

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
	m_UvTexture->TexUnit(m_Shader, "u_Texture", 0);
	m_GroundTexture = new Texture2D("assets/Textures/ground-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_RockTexture = new Texture2D("assets/Textures/rock-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_SnowTexture = new Texture2D("assets/Textures/snow-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void GameLayer::OnUpdate(float dt)
{
	static float t = 0.0f; t += dt;

	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float asp = (float)Application::Get().GetWindow()->GetWidth() / (float)Application::Get().GetWindow()->GetHeight();
	float fov = 45.0f, nearPlane = 0.1f, farPlane = 1000.0f;

	m_Camera->UpdateMatrix(fov, asp, nearPlane, farPlane);
	if (!Application::Get().IsCursor())
		m_Camera->Update(dt);

	m_TessellationShader->Use();
	m_TessellationShader->SetUniform("u_ViewProj", m_Camera->GetMatrix());
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

	m_TessellationShader->Use();
	m_HeightMap->Bind(0);
	m_GroundTexture->Bind(1);
	m_RockTexture->Bind(2);
	m_SnowTexture->Bind(3);
	m_TessellationShader->SetUniform("u_MaxLevel", m_MaxHeight);
	m_TessellationShader->SetUniform("u_GrassLevel", m_GrassLevel);
	m_TessellationShader->SetUniform("u_RockLevel", m_RockLevel);
	m_TessellationShader->SetUniform("u_SnowLevel", m_SnowLevel);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-(float)planeSize/2.0f, 0, -(float)planeSize / 2.0f));
	m_TessellationShader->SetUniform("u_Model", model);
	m_TessellationShader->SetUniform("u_CameraPosition", m_Camera->GetPosition());
	m_Plane->Render();

	m_Skybox->Render(m_Camera);
}

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

void GameLayer::OnImGuiRender()
{
	ImGui::Begin("Info");
	ImGui::DragFloat3("Camera position", &m_Camera->GetPosition()[0], 0.01f, -500.0f, 500.0f);
	ImGui::DragFloat3("Camera orientation", &m_Camera->GetOrientation()[0], 0.01f, -0.99f, 0.99f);
	ImGui::End();

	ImGui::Begin("Height map");
	if (ImGui::Button("Generate")) GenerateHeightMap();
	static int seedInput = seed;
	if (ImGui::SliderInt("Seed", &seedInput, 0, 10e4))
	{
		seed = seedInput;
		perlin.reseed(seed);
		GenerateHeightMap();
	}
	if (ImGui::SliderFloat("Frequency", &frequency, 0.0001, 0.01)) GenerateHeightMap();
	if (ImGui::SliderInt("Octaves", &octaves, 1, 8)) GenerateHeightMap();
	ImVec2 uv_min = ImVec2(0.0f, 1.0f); // Top-left
	ImVec2 uv_max = ImVec2(1.0f, 0.0f); // Lower-right
	float my_tex_w = 200.0f;
	float my_tex_h = 200.0f;
	ImGui::Image((ImTextureID)m_HeightMap->GetId(), ImVec2(my_tex_w, my_tex_h), uv_min, uv_max);
	ImGui::End();

	ImGui::Begin("Tessellation");
	ImGui::SliderFloat("MaxHeight", &m_MaxHeight, 0.0f, 100.f);
	ImGui::SliderFloat("GrassLevel", &m_GrassLevel, 0.0f, 1.f);
	ImGui::SliderFloat("RockLevel", &m_RockLevel, 0.0f, 1.f);
	ImGui::SliderFloat("SnowLevel", &m_SnowLevel, 0.0f, 1.f);
	ImGui::End();

	static bool show = true;
	ImGui::ShowDemoWindow(&show);
}

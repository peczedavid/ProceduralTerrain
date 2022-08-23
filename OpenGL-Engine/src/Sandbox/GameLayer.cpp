#include "Sandbox/GameLayer.h"
#include "Core/Application.h"
#include <glad/glad.h>
#include <glm/gtx/transform.hpp>
#include <imgui.h>

GameLayer::GameLayer()
{
	m_Shader = new Shader("src/Rendering/Shaders/default.vert", "src/Rendering/Shaders/default.frag");
	m_Camera = new Camera(glm::vec3(0, 2, 5), glm::vec3(0, -0.35f, -1.0f));

	Shader* skyboxShader = new Shader("src/Rendering/Shaders/skybox.vert", "src/Rendering/Shaders/skybox.frag");
	m_Skybox = new Skybox(skyboxShader);

	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ebo);

	glBindVertexArray(m_Vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);

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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
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

	m_UvTexture = new Texture2D("assets/Textures/uv-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
	m_UvTexture->TexUnit(m_Shader, "u_Texture", 0);
}

void GameLayer::OnUpdate(float dt)
{
	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float asp = (float)Application::Get().GetWindow()->GetWidth() / (float)Application::Get().GetWindow()->GetHeight();
	float fov = 45.0f, nearPlane = 0.1f, farPlane = 100.0f;

	m_Camera->UpdateMatrix(fov, asp, nearPlane, farPlane);
	if(!Application::Get().IsCursor())
		m_Camera->Update(dt);

	m_Shader->Use();
	m_Shader->SetUniform("u_ViewProj", m_Camera->GetMatrix());

	glBindVertexArray(m_Vao);
	static float t = 0.0f; t += dt;
	static float scale = 1.0f;
	scale = sinf(t * 2.0f) * 0.4f + 1.0f;
	glm::mat4 model = glm::scale(glm::vec3(scale, scale, scale));
	static float rot = 0.0f;
	rot += 0.65f * dt;
	model = glm::rotate(model, rot, glm::vec3(0.0f, 1.0f, 0.0f));
	m_Shader->SetUniform("u_Model", model);
	m_UvTexture->Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	m_Skybox->Render(m_Camera);

}

void GameLayer::OnImGuiRender()
{
	ImGui::Begin("Info");
	ImGui::DragFloat3("Camera position", &m_Camera->GetPosition()[0], 0.01f, -500.0f, 500.0f);
	ImGui::DragFloat3("Camera orientation", &m_Camera->GetOrientation()[0], 0.01f, -0.99f, 0.99f);
	ImGui::End();
	
	static bool show = true;
	ImGui::ShowDemoWindow(&show);
}

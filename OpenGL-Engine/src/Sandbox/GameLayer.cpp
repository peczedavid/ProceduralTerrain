#include "Sandbox/GameLayer.h"
#include "Core/Application.h"
#include <glad/glad.h>
#include <glm/gtx/transform.hpp>

GameLayer::GameLayer()
{
	m_Shader = new Shader("src/Rendering/Shaders/default.vert", "src/Rendering/Shaders/default.frag");
	m_Camera = new Camera(glm::vec3(0, 0, 2));

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

	m_UvTexture = new Texture2D("assets/Textures/uv-texture.png", GL_LINEAR, GL_REPEAT, GL_RGBA, GL_UNSIGNED_BYTE);
	m_UvTexture->TexUnit(m_Shader, "u_Texture", 0);
}

void GameLayer::OnUpdate(float dt)
{
	glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_Shader->Use();

	m_Camera->UpdateMatrix(45.0f, 0.1f, 100.0f);
	if(!Application::Get().IsCursor())
		m_Camera->Update(dt);
	m_Shader->SetUniform("u_ViewProj", m_Camera->GetMatrix());

	static float rot = 0.0f;
	rot += 0.65f * dt;
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0.0f, 1.0f, 0.0f));
	m_Shader->SetUniform("u_Model", model);
	m_UvTexture->Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

#include "Rendering/Geometry/Skybox.h"
#include <glad/glad.h>
#include <string>
#include <stb_image.h>
#include <glm/gtx/transform.hpp>

Skybox::Skybox(ShaderBase* shader)
	: m_Shader(shader)
{
	float vertices[27] =
	{
		-1.0f, -1.0f,  1.0f, //       7--------6
		 1.0f, -1.0f,  1.0f, //      /|       /|
		 1.0f, -1.0f, -1.0f, //     4--------5 |
		-1.0f, -1.0f, -1.0f, //     | |      | |
		-1.0f,  1.0f,  1.0f, //     | 3------|-2
		 1.0f,  1.0f,  1.0f, //     |/       |/
		 1.0f,  1.0f, -1.0f, //     0--------1
		-1.0f,  1.0f, -1.0f  //
	};

	uint32_t indices[36] =
	{
		// Right
		1, 2, 6,
		6, 5, 1,
		// Left
		0, 4, 7,
		7, 3, 0,
		// Top
		4, 5, 6,
		6, 7, 4,
		// Bottom
		0, 3, 2,
		2, 1, 0,
		// Back
		0, 1, 5,
		5, 4, 0,
		// Front
		3, 7, 6,
		6, 2, 3
	};

	glGenVertexArrays(1, &m_Vao);
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ebo);

	glBindVertexArray(m_Vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::string facesPath[6] =
	{
		/*"assets/Textures/Skybox/skybox_px.png",
		"assets/Textures/Skybox/skybox_nx.png",
		"assets/Textures/Skybox/skybox_py.png",
		"assets/Textures/Skybox/skybox_ny.png",
		"assets/Textures/Skybox/skybox_pz.png",
		"assets/Textures/Skybox/skybox_nz.png",*/
		"assets/Textures/Skybox/right.jpg",
		"assets/Textures/Skybox/left.jpg",
		"assets/Textures/Skybox/top.jpg",
		"assets/Textures/Skybox/bottom.jpg",
		"assets/Textures/Skybox/front.jpg",
		"assets/Textures/Skybox/back.jpg",
	};

	glGenTextures(1, &m_TextureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (uint32_t i = 0; i < 6; i++)
	{
		int width, height, colorCh;
		unsigned char* bytes = stbi_load(facesPath[i].c_str(), &width, &height, &colorCh, 0);
		if (bytes)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				bytes
			);
			stbi_image_free(bytes);
		}
		else
		{
			printf("Failed to load in texture: %s", facesPath[0].c_str());
			stbi_image_free(bytes);
		}
	}
	m_Shader->Use();
	uint32_t textureUnit = glGetUniformLocation(m_Shader->GetProgramId(), "u_Skybox");
	glUniform1i(textureUnit, 0);
}

Skybox::~Skybox()
{
}

void Skybox::Render(Camera* camera)
{
	// The skybox is on the edge
	glDepthFunc(GL_LEQUAL);
	m_Shader->Use();
	m_Shader->SetUniform("u_View", glm::mat4(glm::mat3(camera->GetView())));
	m_Shader->SetUniform("u_Proj", camera->GetProj());

	glBindVertexArray(m_Vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glDepthFunc(GL_LESS);
}

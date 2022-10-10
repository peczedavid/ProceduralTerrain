#include "pch.h"

#include "Rendering/Geometry/Skybox.h"
#include <glad/glad.h>

Skybox::Skybox(Ref<Shader> shader)
	: m_Shader(shader)
{
	constexpr float vertices[27] =
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

	constexpr uint32_t indices[36] =
	{
		// Right
		6, 2, 1,
		1, 5, 6,
		// Left
		7, 4, 0,
		0, 3, 7,
		// Top
		6, 5, 4,
		4, 7, 6,
		// Bottom
		2, 3, 0,
		0, 1, 2,
		// Back
		5, 1, 0,
		0, 4, 5,
		// Front
		6, 7, 3,
		3, 2, 6
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

	constexpr const char* facesPath[6] =
	{
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
		unsigned char* bytes = stbi_load(facesPath[i], &width, &height, &colorCh, 0);
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
			printf("Failed to load in texture: %s", facesPath[0]);
			stbi_image_free(bytes);
		}
	}
	m_Shader->Use();
}

Skybox::~Skybox()
{
}

void Skybox::Render()
{
	// The skybox is on the edge
	glDepthFunc(GL_LEQUAL);
	m_Shader->Use();

	glBindVertexArray(m_Vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glDepthFunc(GL_LESS);
}

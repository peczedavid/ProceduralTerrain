#include "Texture2D.h"
#include <stb_image.h>

Texture2D::Texture2D(const char* path, uint32_t filter, uint32_t wrap, uint32_t format, uint32_t pixelType)
	: m_Format(format)
{
	int width, height, colorCh;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* bytes = stbi_load(path, &width, &height, &colorCh, 0);
	m_Width = width;
	m_Height = height;
	m_ColorChannels = colorCh;

	glGenTextures(1, &m_Id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, format, pixelType, bytes);
	glGenerateMipmap(m_Id);
	stbi_image_free(bytes);
}

Texture2D::~Texture2D()
{
}

void Texture2D::TexUnit(Shader* shader, const char* uniform, uint32_t slot)
{
	uint32_t textureUnit = glGetUniformLocation(shader->GetProgramId(), uniform);
	glUniform1i(textureUnit, slot);
}

void Texture2D::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_Id);
}

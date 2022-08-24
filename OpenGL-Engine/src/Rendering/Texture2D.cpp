#include "Texture2D.h"
#include <stb_image.h>

Texture2D::Texture2D(const char* path, GLenum filter, GLenum wrap, GLenum internalFormat, GLenum format, GLenum pixelType)
	: m_Format(format), m_InternalFormat(internalFormat)
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

	glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_Format, pixelType, bytes);
	glGenerateMipmap(m_Id);
	stbi_image_free(bytes);
}

Texture2D::Texture2D(uint32_t width, uint32_t height, GLenum filter, GLenum wrap, GLenum internalFormat)
	: m_Width(width), m_Height(height), m_InternalFormat(internalFormat)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_Id);
	glBindTexture(GL_TEXTURE_2D, m_Id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTextureStorage2D(m_Id, 1, internalFormat, m_Width, m_Height);
	glBindImageTexture(0, m_Id, 0, GL_FALSE, 0, GL_WRITE_ONLY, internalFormat);
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

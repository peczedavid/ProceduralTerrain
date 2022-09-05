#pragma once
#include "Rendering/Shaders/Shader.h"

class Texture2D
{
public:
	Texture2D(const char* path, GLenum filter, GLenum wrap, GLenum internalFormat, GLenum format, GLenum pixelType);
	Texture2D(uint32_t width, uint32_t height, GLenum filter, GLenum wrap, GLenum internalFormat);
	~Texture2D();

	void LoadData(unsigned char* bytes, GLenum internalFormat, GLenum format);

	//void TexUnit(Shader* shader, const char* uniform, uint32_t slot);
	void Bind(uint32_t slot = 0u);
	void BindImage();
private:
	uint32_t m_Id;
	uint32_t m_Width, m_Height, m_ColorChannels;
	uint32_t m_InternalFormat;

public:
	inline uint32_t GetId() { return m_Id; }
	inline const uint32_t GetId() const { return m_Id; }

	inline uint32_t GetWidth() { return m_Width; }
	inline const uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() { return m_Height; }
	inline const uint32_t GetHeight() const { return m_Height; }
	inline uint32_t ColorChannels() { return m_ColorChannels; }
	inline const uint32_t ColorChannels() const { return m_ColorChannels; }

	inline uint32_t GetInternalFormat() { return m_InternalFormat; }
	inline const uint32_t GetInternalFormat() const { return m_InternalFormat; }
};

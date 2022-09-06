#pragma once

#include <cstdint>

class FrameBuffer
{
public:
	FrameBuffer(uint32_t width, uint32_t height);
	~FrameBuffer();

	void Bind();
	void Resize(uint32_t width, uint32_t height);

	static void Default();
private:
	uint32_t m_FrameBufferId, m_RenderBufferId;
	uint32_t m_TextureId;
public:
	inline uint32_t GetTextureId() { return m_TextureId; }
	inline const uint32_t GetTextureId() const { return m_TextureId; }
};

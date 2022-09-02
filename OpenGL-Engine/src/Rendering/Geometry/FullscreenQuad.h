#pragma once

#include <cstdint>

class FullscreenQuad
{
public:
	FullscreenQuad();
	~FullscreenQuad();
	
	void Render();
private:
	uint32_t m_Vao, m_Vbo;
};

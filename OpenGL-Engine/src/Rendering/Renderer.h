#pragma once
#include <glad/glad.h>

namespace Renderer
{
	extern bool debugAxis;

	extern bool wireframe;

	void TogglePolygonMode();

	extern bool multisample;
	void ToggleMSAA();

	void SetOpenGLConfig();
};

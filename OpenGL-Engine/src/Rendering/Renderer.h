#pragma once
#include <glad/glad.h>

namespace Renderer
{
	extern bool debugView;

	extern bool wireframe;

	void TogglePolygonMode();

	void SetOpenGLConfig();
};

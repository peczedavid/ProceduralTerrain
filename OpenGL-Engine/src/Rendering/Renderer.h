#pragma once
#include <glad/glad.h>

// Might become a namespace
namespace Renderer
{
	static bool wireframe = true;

	static void TogglePolygonMode()
	{
		wireframe = !wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
	}

	static void SetOpenGLConfig()
	{
		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
	}
};

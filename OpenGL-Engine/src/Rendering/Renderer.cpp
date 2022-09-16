#include "Rendering/Renderer.h"
#include <algorithm>

namespace Renderer
{
	bool Renderer::debugView = false;

	bool Renderer::wireframe = false;

	const size_t fpsPoolSize = 600;
	size_t poolIndex = 0;
	std::vector<float> fpsPool;
	float maxFPS = 0;

	void Initialize()
	{
		SetOpenGLConfig();
		InitPool();
	}

	void TogglePolygonMode()
	{
		wireframe = !wireframe;
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
	}

	void SetOpenGLConfig()
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
	}

	void InitPool()
	{
		fpsPool.resize(fpsPoolSize);
	}

	void AddFPSSample(float fps)
	{
		for (size_t i = 0; i < fpsPoolSize - 1; i++)
		{
			fpsPool.at(i) = fpsPool.at(i + 1);
			fpsPool[fpsPoolSize - 1] = fps;
		}
	}
}
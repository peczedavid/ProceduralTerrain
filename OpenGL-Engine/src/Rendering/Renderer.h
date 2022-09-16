#pragma once
#include <glad/glad.h>
#include <cinttypes>
#include <vector>

namespace Renderer
{
	void Initialize();

	extern bool debugView;

	extern bool wireframe;

	void TogglePolygonMode();

	void SetOpenGLConfig();

	extern const size_t fpsPoolSize;
	extern size_t poolIndex;
	extern std::vector<float> fpsPool;
	extern float maxFPS;

	void InitPool();

	void AddFPSSample(float FPS);
};

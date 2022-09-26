#pragma once
#include <glad/glad.h>
#include <cinttypes>
#include <vector>
#include "Rendering/FPSPool.h"

class Renderer
{
public:
	static void Initialize();
	static void TogglePolygonMode();
	static void SetOpenGLConfig();

	static const bool IsWireFrame();
public:
	static FPSPool FPSPool;
	static bool DebugView;
private:
	static bool WireFrame;
};
#pragma once
#include <glad/glad.h>
#include <cinttypes>
#include <vector>
#include "Rendering/FPSPool.h"
#include "Core/Core.h"
#include "Rendering/Texture2D.h"

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
	static bool Grid;
public:
	static Ref<Texture2D> s_PlaceHolderTexture;
private:
	static bool WireFrame;
};
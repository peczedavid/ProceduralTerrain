#include "Rendering/Renderer.h"
#include <algorithm>

bool Renderer::DebugView = false;
bool Renderer::WireFrame = false;
FPSPool Renderer::FPSPool;

void Renderer::Initialize()
{
	SetOpenGLConfig();
}

void Renderer::TogglePolygonMode()
{
	WireFrame = !WireFrame;
	glPolygonMode(GL_FRONT_AND_BACK, WireFrame ? GL_LINE : GL_FILL);
}

void Renderer::SetOpenGLConfig()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, WireFrame ? GL_LINE : GL_FILL);
}

const bool Renderer::IsWireFrame()
{
	return WireFrame;
}
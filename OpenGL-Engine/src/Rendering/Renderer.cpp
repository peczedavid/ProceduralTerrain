#include "pch.h"

#include "Rendering/Renderer.h"
#include <Rendering/Texture2D.h>

bool Renderer::DebugView = false;
bool Renderer::WireFrame = false;
bool Renderer::Grid = false;
FPSPool Renderer::FPSPool;

static void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:
		ERROR("OpenGL HIGH severity message: %s", message);
		break;

		case GL_DEBUG_SEVERITY_MEDIUM:
		WARN("OpenGL MEDIUM severity message: %s", message);
		break;

		case GL_DEBUG_SEVERITY_LOW:
		INFO("OpenGL LOW severity message: %s", message);
		break;
	}
}

Ref<Texture2D> Renderer::s_PlaceHolderTexture;

void Renderer::Initialize()
{
	SetOpenGLConfig();
	glDebugMessageCallback(DebugMessageCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	s_PlaceHolderTexture = CreateRef<Texture2D>("assets/Textures/placeholder.png", GL_NEAREST, GL_REPEAT, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
}

void Renderer::TogglePolygonMode()
{
	WireFrame = !WireFrame;
	glPolygonMode(GL_FRONT_AND_BACK, WireFrame ? GL_LINE : GL_FILL);
}

void Renderer::SetOpenGLConfig()
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, WireFrame ? GL_LINE : GL_FILL);
}

const bool Renderer::IsWireFrame()
{
	return WireFrame;
}
#include "pch.h"

#include "Rendering/Renderer.h"

bool Renderer::DebugView = false;
bool Renderer::WireFrame = false;
FPSPool Renderer::FPSPool;

static void DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:
		ERROR("OpenGL HIGH severity message: {0}", message);
		break;

		case GL_DEBUG_SEVERITY_MEDIUM:
		WARN("OpenGL MEDIUM severity message: {0}", message);
		break;

		case GL_DEBUG_SEVERITY_LOW:
		INFO("OpenGL LOW severity message: {0}", message);
		break;
	}
}

void Renderer::Initialize()
{
	SetOpenGLConfig();
	glDebugMessageCallback(DebugMessageCallback, nullptr);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
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
	//glFrontFace(GL_CW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, WireFrame ? GL_LINE : GL_FILL);
}

const bool Renderer::IsWireFrame()
{
	return WireFrame;
}
#pragma once

#include <glm/glm.hpp>
#include "Rendering/Texture2D.h"
#include "Core/Core.h"
#include "Rendering/Shaders/Shader.h"

struct PBRMaterial
{
	// Shader
	Ref<Shader> Shader;

	// Color
	glm::vec3 Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	Ref<Texture2D> AlbedoMap;
	bool UseAlbedoMap;

	// Modifiers
	glm::vec3 F0 = glm::vec3(0.04f, 0.04f, 0.04f);
	float Metallic = 0.0f;
	float Roughness = 0.5f;
	float AmbientOcclusion = 0.5f;

	void SetUniforms()
	{

	}
};
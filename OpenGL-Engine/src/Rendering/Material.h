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
	bool UseAlbedoMap = false;

	// Modifiers
	float F0 = 0.04f;
	float Metallic = 0.0f;
	float Roughness = 0.5f;
	float AmbientOcclusion = 0.3f;

	void SetUniforms()
	{
		Shader->Use();
		Shader->SetUniform("u_Albedo", Albedo);
		Shader->SetUniform("u_AmbientOcclusion", AmbientOcclusion);
		Shader->SetUniform("u_Roughness", Roughness);
		Shader->SetUniform("u_F0", F0);
		Shader->SetUniform("u_Metallic", Metallic);
	}
};
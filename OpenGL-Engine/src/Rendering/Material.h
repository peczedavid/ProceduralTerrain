#pragma once

#include <glm/glm.hpp>

struct PBRMaterial
{
	glm::vec3 Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 F0 = glm::vec3(0.04f, 0.04f, 0.04f);
	float Metallic = 0.0f;
	float Roughness = 0.5f;
	float AmbientOcclusion = 0.5f;
};
#pragma once

#include <glm/glm.hpp>
#include "Rendering/Texture2D.h"
#include "Core/Core.h"
#include "Rendering/Shaders/Shader.h"

struct Material {

	virtual void SetUniforms() = 0;
	virtual Ref<Shader> GetShader() = 0;
	virtual void SetShader(Ref<Shader> shader) = 0;

	virtual void DrawImGui() {}
};

struct PBRMaterial : Material
{
private:
	// Shader
	Ref<Shader> m_Shader;
public:
	// Color
	glm::vec3 Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
	Ref<Texture2D> AlbedoMap;
	bool UseAlbedoMap = false;

	// Modifiers
	float F0 = 0.04f;
	float Metallic = 0.0f;
	float Roughness = 0.5f;
	float AmbientOcclusion = 0.3f;

	Ref<Shader> GetShader() override;
	void SetShader(Ref<Shader> shader) override;

	void SetUniforms() override;
	void DrawImGui() override;
};
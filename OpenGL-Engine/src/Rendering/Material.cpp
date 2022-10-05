#include "pch.h"

#include "Rendering/Material.h"

Ref<Shader> PBRMaterial::GetShader()
{
	return m_Shader;
}

void PBRMaterial::SetShader(Ref<Shader> shader)
{
	m_Shader = shader;
}

void PBRMaterial::SetUniforms()
{
	m_Shader->Use();
	m_Shader->SetUniform("u_Albedo", Albedo);
	m_Shader->SetUniform("u_AmbientOcclusion", AmbientOcclusion);
	m_Shader->SetUniform("u_Roughness", Roughness);
	m_Shader->SetUniform("u_F0", F0);
	m_Shader->SetUniform("u_Metallic", Metallic);
}

void PBRMaterial::DrawImGui()
{
	ImGui::ColorEdit3("Abledo", &Albedo[0], 0);
	ImGui::DragFloat("Roughness", &Roughness, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("Metallic", &Metallic, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("Ambient occlusion", &AmbientOcclusion, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("F0", &F0, 0.0005f, 0.0f, 1.0f);
}

#include "pch.h"

#include "Rendering/Material.h"
#include "Sandbox/GameLayerImGui.h"

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
	if (UseAlbedoMap && AlbedoMap.get())
	{
		m_Shader->SetUniform("u_UseAlbedoMap", 1);
		AlbedoMap->Bind(0);
	}
	else
	{
		m_Shader->SetUniform("u_UseAlbedoMap", 0);
	}
	m_Shader->SetUniform("u_Albedo", Albedo);
	m_Shader->SetUniform("u_AmbientOcclusion", AmbientOcclusion);
	m_Shader->SetUniform("u_Roughness", Roughness);
	m_Shader->SetUniform("u_F0", F0);
	m_Shader->SetUniform("u_Metallic", Metallic);
}


void PBRMaterial::DrawImGui()
{
	ImGui::SetNextItemOpen(true);
	if (ImGui::CollapsingHeader("Albedo"))
	{
		ImGui::Image((ImTextureID)(AlbedoMap.get() ? AlbedoMap->GetId() : GameLayerImGui::s_PlaceHolderTexture->GetId()), ImVec2(50.0f, 50.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		ImGui::Checkbox("Use", &UseAlbedoMap);
		ImGui::SameLine();
		ImGui::ColorEdit3("Color", &Albedo[0], ImGuiColorEditFlags_NoInputs);
	}

	ImGui::SetNextItemOpen(true);
	if (ImGui::CollapsingHeader("Rougness"))
	{
		ImGui::Image((ImTextureID)GameLayerImGui::s_PlaceHolderTexture->GetId(), ImVec2(50.0f, 50.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		static bool placeholder = false;
		ImGui::Checkbox("Use", &placeholder);
		ImGui::SameLine();
		ImGui::PushID(100);
		ImGui::SliderFloat("", &Roughness, 0.0f, 1.0f);
		ImGui::PopID();
	}

	ImGui::SetNextItemOpen(true);
	if (ImGui::CollapsingHeader("Metallic"))
	{
		ImGui::Image((ImTextureID)GameLayerImGui::s_PlaceHolderTexture->GetId(), ImVec2(50.0f, 50.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		static bool placeholder = false;
		ImGui::Checkbox("Use", &placeholder);
		ImGui::SameLine();
		ImGui::PushID(101);
		ImGui::SliderFloat("", &Metallic, 0.0f, 1.0f);
		ImGui::PopID();
	}

	ImGui::SetNextItemOpen(true);
	if (ImGui::CollapsingHeader("Ambient occlusion"))
	{
		ImGui::Image((ImTextureID)GameLayerImGui::s_PlaceHolderTexture->GetId(), ImVec2(50.0f, 50.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		static bool placeholder = false;
		ImGui::Checkbox("Use", &placeholder);
		ImGui::SameLine();
		ImGui::PushID(102);
		ImGui::SliderFloat("", &AmbientOcclusion, 0.0f, 1.0f);
		ImGui::PopID();

	}

	ImGui::SetNextItemOpen(true);
	if (ImGui::CollapsingHeader("F0"))
	{
		ImGui::Image((ImTextureID)GameLayerImGui::s_PlaceHolderTexture->GetId(), ImVec2(50.0f, 50.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::SameLine();
		static bool placeholder = false;
		ImGui::Checkbox("Use", &placeholder);
		ImGui::SameLine();
		ImGui::PushID(103);
		ImGui::SliderFloat("", &F0, 0.0f, 1.0f);
		ImGui::PopID();
	}
}

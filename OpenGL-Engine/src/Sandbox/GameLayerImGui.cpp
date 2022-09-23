#include "Sandbox/GameLayerImGui.h"
#include <imgui.h>
#include <Windows.h>
#include <dxgi1_4.h>
#include "Rendering/Renderer.h"
#include "Core/Application.h"

GameLayerImGui::GameLayerImGui(GameLayer* gameLayer)
	: m_GameLayer(gameLayer)
{
}

void GameLayerImGui::ViewportPanel()
{
	if (ImGui::Begin("Viewport"))
	{
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		float my_tex_w = viewportPanelSize.x;
		float my_tex_h = viewportPanelSize.y;
		if (m_GameLayer->m_ViewportSize.x != viewportPanelSize.x || m_GameLayer->m_ViewportSize.y != viewportPanelSize.y)
		{
			m_GameLayer->m_FrameBuffer->Resize(my_tex_w, my_tex_h);
			m_GameLayer->m_Camera->Resize(my_tex_w, my_tex_h);
			m_GameLayer->m_ViewportSize.x = viewportPanelSize.x;
			m_GameLayer->m_ViewportSize.y = viewportPanelSize.y;
		}
		else
		{
			ImVec2 uv_min = ImVec2(0.0f, 1.0f); // Top-left
			ImVec2 uv_max = ImVec2(1.0f, 0.0f); // Lower-right
			ImGui::Image((ImTextureID)m_GameLayer->m_FrameBuffer->GetTextureId(), ImVec2(my_tex_w, my_tex_h), uv_min, uv_max);
		}
	}
	ImGui::End();
}

void GameLayerImGui::NoisePanel()
{
	if (ImGui::Begin("Noise props"))
	{
		ImGui::SliderFloat("Amlitude", &m_GameLayer->m_Amplitude, 0.01f, 1.0f);
		ImGui::SliderFloat("Frequency", &m_GameLayer->m_Frequency, 0.01f, 5.0f);
		ImGui::SliderFloat("Gain", &m_GameLayer->m_Gain, 0.01f, 0.5f);
		ImGui::SliderFloat("Scale", &m_GameLayer->m_Scale, 0.001f, 0.3f);
		ImGui::SliderFloat2("NoiseOffset", &m_GameLayer->m_NoiseOffset[0], 0.0f, 10.0f);
		if (ImGui::Button("Generate")) m_GameLayer->GenerateTerrain();
	}
	ImGui::End();
}

void GameLayerImGui::LandscapePanel()
{
	if (ImGui::Begin("Landscape"))
	{
		ImGui::SliderFloat("MaxHeight", &m_GameLayer->m_MaxHeight, 0.0f, 1000.f);
		ImGui::SliderFloat("FogGradient", &m_GameLayer->m_FogGradient, 0.0f, 5.f);
		ImGui::SliderFloat("FogDensity", &m_GameLayer->m_FogDensity, 0.0f, 0.01f);
		ImGui::Checkbox("Normals", &m_GameLayer->m_TerrainNormals);
	}
	ImGui::End();
}

void GameLayerImGui::WaterPanel()
{
	if (ImGui::Begin("Water"))
	{
		ImGui::SliderFloat("Level", &m_GameLayer->m_WaterLevel, -50.0f, 100.0f);
		ImGui::Checkbox("Normals", &m_GameLayer->m_WaterNormals);
		ImGui::SliderFloat("A - Steepness", &m_GameLayer->m_WaveA[2], 0.0f, 1.0f);
		ImGui::SliderFloat("A - Wavelength", &m_GameLayer->m_WaveA[3], 10.0f, 75.0f);
		ImGui::SliderFloat2("A - Direction", &m_GameLayer->m_WaveA[0], -1.0f, 1.0f);
		ImGui::SliderFloat("B - Steepness", &m_GameLayer->m_WaveB[2], 0.0f, 1.0f);
		ImGui::SliderFloat("B - Wavelength", &m_GameLayer->m_WaveB[3], 10.0f, 75.0f);
		ImGui::SliderFloat2("B - Direction", &m_GameLayer->m_WaveB[0], -1.0f, 1.0f);
		ImGui::SliderFloat("C - Steepness", &m_GameLayer->m_WaveC[2], 0.0f, 1.0f);
		ImGui::SliderFloat("C - Wavelength", &m_GameLayer->m_WaveC[3], 10.0f, 75.0f);
		ImGui::SliderFloat2("C - Direction", &m_GameLayer->m_WaveC[0], -1.0f, 1.0f);
	}
	ImGui::End();
}

void GameLayerImGui::ContolsPanel()
{
	if (ImGui::Begin("Controls"))
	{
		ImGui::Text("WASD - Move");
		ImGui::Text("Mouse - Look around");
		ImGui::Text("Space - Move up");
		ImGui::Text("C - Move down");
		ImGui::Text("Shift - Fast");
		ImGui::Text("Ctrl - Slow");
		ImGui::Text("F2 - Screenshot");
		ImGui::Text("F3 - Debug info");
		ImGui::Text("F - Wireframe");
		ImGui::Text("Tab - Cursor");
		ImGui::Text("Esc - Close");
	}
	ImGui::End();
}

void GameLayerImGui::VendorInfoPanel()
{
	static bool firstRun = true;
	static IDXGIFactory4* pFactory{};
	static IDXGIAdapter3* adapter{};
	static DXGI_QUERY_VIDEO_MEMORY_INFO videoMemoryInfo{};
	if (firstRun)
	{
		CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory);
		pFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(&adapter));
		firstRun = false;
	}
	if (ImGui::Begin("Vendor info"))
	{
		adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);
		const size_t usedVRAM = videoMemoryInfo.CurrentUsage / 1024 / 1024;
		const size_t maxVRAM = videoMemoryInfo.Budget / 1024 / 1024;
		ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
		ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
		ImGui::Text("Version: %s", glGetString(GL_VERSION));
		int versionMajor, versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);
		ImGui::Text("OpenGL %d.%d", versionMajor, versionMinor);
		ImGui::Text("VRAM: %d/%d MB", usedVRAM, maxVRAM);
	}
	ImGui::End();
}

void GameLayerImGui::DebugOverlayPanel()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("Debug overaly", (bool*)1, window_flags);
	ImGui::Text("Viewport size: %.0fx%.0f", m_GameLayer->m_ViewportSize.x, m_GameLayer->m_ViewportSize.y);
	const glm::vec3 cameraPos = m_GameLayer->m_Camera->GetPosition();
	ImGui::Text("XYZ: %.2f / %.2f / %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
	const glm::vec3 cameraOri = m_GameLayer->m_Camera->GetOrientation();
	ImGui::Text("Facing: %.2f / %.2f / %.2f", cameraOri.x, cameraOri.y, cameraOri.z);
	ImGui::Text("%d FPS (max: %.0f)", m_GameLayer->m_FPS, Renderer::FPSPool.GetMax());
	ImGui::Text("%.4f ms", 1.0f / m_GameLayer->m_FPS);
	const auto& samples = Renderer::FPSPool.GetSamples();
	ImGui::PlotLines("FPS", &samples[0], samples.size(), 0, 0, 0, Renderer::FPSPool.GetMax(), ImVec2(175, 40));
	ImGui::End();
	ImGui::PopStyleVar();
}

void GameLayerImGui::TexturesPanel()
{
	if (ImGui::Begin("Textures"))
	{
		for (int i = 2; i >= 0; i--)
		{
			for (int j = 0; j < 3; j++)
			{
				DrawImage(m_GameLayer->m_HeightMaps[i * 3 + j]->GetId());
				if (j < 2) ImGui::SameLine();
			}
		}
	}
	ImGui::End();
}

void GameLayerImGui::FFTPanel()
{
	if (ImGui::Begin("FFT textures"))
	{
		DrawImage(m_GameLayer->m_H0k->GetId());
		ImGui::SameLine();
		DrawImage(m_GameLayer->m_H0minusk->GetId());
		ImGui::SameLine();
		DrawImage(m_GameLayer->m_TwiddleTexture->GetId());
		
		DrawImage(m_GameLayer->m_HtDy->GetId());
		ImGui::SameLine();
		DrawImage(m_GameLayer->m_HtDx->GetId());
		ImGui::SameLine();
		DrawImage(m_GameLayer->m_HtDz->GetId());

		DrawImage(m_GameLayer->m_PingPong0->GetId());
		ImGui::SameLine();
		DrawImage(m_GameLayer->m_PingPong1->GetId());
		ImGui::SameLine();
		DrawImage(m_GameLayer->m_Displacement->GetId());
	}
	ImGui::End();
}

void GameLayerImGui::GraphicsSettingsPanel()
{
	if (ImGui::Begin("Graphics settings"))
	{
		auto window = Application::Get().GetWindow();
		bool vSync = window->IsVSync();
		ImGui::Checkbox("VSync", &vSync);
		window->SetVSync(vSync);
	}
	ImGui::End();
}

int id = 0;

void DrawShader(const char* name, Shader* shader)
{
	ImGui::Text(name);
	ImGui::SameLine();
	ImGui::PushID(id++);
	if (ImGui::Button("Reload"))
		shader->ReCompile();
	ImGui::PopID();
}

void GameLayerImGui::ShadersPanel()
{
	if (ImGui::Begin("Shaders"))
	{
		id = 0;
		DrawShader("Axis", m_GameLayer->m_Axis->m_Shader);
		DrawShader("Skybox", m_GameLayer->m_Skybox->m_Shader);
		DrawShader("Terrain compute", m_GameLayer->m_TerrainComputeShader);
		DrawShader("Terrain tess", m_GameLayer->m_TerrainShader);
	}
	ImGui::End();
}

void GameLayerImGui::DrawImage(uint32_t textureId, float my_tex_w, float my_tex_h)
{
	ImGuiIO& io = ImGui::GetIO();
	ImTextureID my_tex_id = (ImTextureID)textureId;
	{
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 uv_min = ImVec2(0.0f, 1.0f); // Top-left
		ImVec2 uv_max = ImVec2(1.0f, 0.0f); // Lower-right
		ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			float region_sz = 32.0f;
			float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
			float region_y = io.MousePos.y - pos.y;
			region_y = my_tex_h - region_y;
			region_y = region_y - region_sz * 0.5f;
			float zoom = 7.0f;
			if (region_x < 0.0f) { region_x = 0.0f; }
			else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
			if (region_y < 0.0f) { region_y = 0.0f; }
			else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }
			ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
			ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
			ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y + region_sz) / my_tex_h);
			ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y) / my_tex_h);
			ImGui::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1);
			ImGui::EndTooltip();
		}
	}
}

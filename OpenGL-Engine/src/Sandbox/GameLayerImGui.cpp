#include "pch.h"

#include "Sandbox/GameLayerImGui.h"
#include <imgui.h>
#include <ImGuizmo.h>
#include <dxgi1_4.h>
#include "Rendering/Renderer.h"
#include "Core/Application.h"
#include "Rendering/Material.h"

GameLayerImGui::GameLayerImGui(GameLayer* gameLayer)
	: m_GameLayer(gameLayer)
{
}

void GameLayerImGui::ViewportPanel()
{
	if (ImGui::Begin("Viewport"))
	{
		const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		const float my_tex_w = viewportPanelSize.x;
		const float my_tex_h = viewportPanelSize.y;
		if (m_GameLayer->m_ViewportSize.x != viewportPanelSize.x || m_GameLayer->m_ViewportSize.y != viewportPanelSize.y)
		{
			m_GameLayer->m_FrameBuffer->Resize(my_tex_w, my_tex_h);
			m_GameLayer->m_Camera->Resize(my_tex_w, my_tex_h);
			m_GameLayer->m_TrackballCamera->Resize(my_tex_w, my_tex_h);
			m_GameLayer->m_ViewportSize.x = viewportPanelSize.x;
			m_GameLayer->m_ViewportSize.y = viewportPanelSize.y;
		}
		else
		{
			const ImVec2 uv_min(0.0f, 1.0f); // Top-left
			const ImVec2 uv_max(1.0f, 0.0f); // Lower-right
			ImGui::Image((ImTextureID)m_GameLayer->m_FrameBuffer->GetTextureId(), ImVec2(my_tex_w, my_tex_h), uv_min, uv_max);

			if (m_SelectedObject)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				auto camera = m_GameLayer->m_Camera.get();
				const glm::mat4 view = camera->GetView();
				const glm::mat4 proj = camera->GetProj();
				glm::mat4 transform = m_SelectedObject->GetTransform();
				const auto originalRotation = m_SelectedObject->GetRotation();

				static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
				static ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;

				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
				{
					if (ImGui::IsKeyDown(ImGuiKey_Q))
					{
						operation = ImGuizmo::OPERATION::TRANSLATE;
						mode = ImGuizmo::MODE::WORLD;
					}
					if (ImGui::IsKeyDown(ImGuiKey_W))
					{
						operation = ImGuizmo::OPERATION::ROTATE;
						mode = ImGuizmo::MODE::LOCAL;
					}
					if (ImGui::IsKeyDown(ImGuiKey_E))
					{
						operation = ImGuizmo::OPERATION::SCALE;
						mode = ImGuizmo::MODE::WORLD;
					}
				}

				ImGuizmo::Manipulate(&view[0][0], &proj[0][0], operation, mode, &transform[0][0]);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					ImGuizmo::DecomposeMatrixToComponents(&transform[0][0], &translation[0], &rotation[0], &scale[0]);
					glm::vec3 deltaRotation = glm::radians(rotation) - originalRotation;
					m_SelectedObject->Set(translation, originalRotation + deltaRotation, scale);
				}
			}
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
		ImGui::Checkbox("Normals", &m_GameLayer->m_TerrainNormals);
		ImGui::Checkbox("Shade", &m_GameLayer->m_ShadeTerrain);
	}
	ImGui::End();
}

void GameLayerImGui::WaterPanel()
{
	if (ImGui::Begin("Water"))
	{
		ImGui::SliderFloat("Level", &m_GameLayer->m_WaterLevel, -50.0f, 100.0f);
		ImGui::SliderFloat("Steepness dropoff", &m_GameLayer->m_SteepnessDropoff, 0.01f, 5.0f);
		ImGui::SliderFloat("Wavelength dropoff", &m_GameLayer->m_WavelengthDropoff, 0.01f, 5.0f);
		ImGui::SliderFloat("Shininess", &m_GameLayer->m_WaterShininess, 10.0f, 600.f);
		ImGui::SliderFloat("Reflectivity", &m_GameLayer->m_WaterReflectivity, 0.001f, 5.0f);
		ImGui::Checkbox("Normals", &m_GameLayer->m_WaterNormals);
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
	static GLubyte* vendor{};
	static GLubyte* renderer{};
	static GLubyte* version{};
	static GLint majorVersion{}, minorVersion{};
	if (firstRun)
	{
		CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&pFactory);
		pFactory->EnumAdapters(0, reinterpret_cast<IDXGIAdapter**>(&adapter));
		vendor = (GLubyte*)glGetString(GL_VENDOR);
		renderer = (GLubyte*)glGetString(GL_RENDERER);
		version = (GLubyte*)glGetString(GL_VERSION);
		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
		ASSERT(majorVersion >= 4, "OpenGL 4.5 or higher is supported!");
		ASSERT(minorVersion >= 5, "OpenGL 4.5 or higher is supported!");
		firstRun = false;
	}
	if (ImGui::Begin("Vendor info"))
	{
		adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &videoMemoryInfo);
		const size_t usedVRAM = videoMemoryInfo.CurrentUsage / 1024 / 1024;
		const size_t maxVRAM = videoMemoryInfo.Budget / 1024 / 1024;
		ImGui::Text("Vendor: %s", vendor);
		ImGui::Text("Renderer: %s", renderer);
		ImGui::Text("Version: %s", version);
		ImGui::Text("OpenGL %d.%d", majorVersion, minorVersion);
		ImGui::Text("VRAM: %d/%d MB", usedVRAM, maxVRAM);
	}
	ImGui::End();
}

void GameLayerImGui::DebugOverlayPanel()
{
	const ImGuiIO& io = ImGui::GetIO();
	const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
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

void GameLayerImGui::GraphicsSettingsPanel()
{
	if (ImGui::Begin("Graphics settings"))
	{
		auto window = Application::Get().GetWindow();
		bool vSync = window->IsVSync();
		ImGui::Checkbox("VSync", &vSync);
		window->SetVSync(vSync);
		if (ImGui::Button("Toggle camera"))
		{
			m_GameLayer->m_SelectedCamera = (m_GameLayer->m_SelectedCamera + 1) % 2;
		}
	}
	ImGui::End();
}

void GameLayerImGui::EnviromentPanel()
{
	if (ImGui::Begin("Enviroment"))
	{
		ImGui::SliderFloat3("Sun direction", &m_GameLayer->m_SunDirection[0], -10.0f, 10.f);
		ImGui::SliderFloat("Fog density", &m_GameLayer->m_FogData[0], 0.0f, 0.01f);
		ImGui::SliderFloat("Fog gradient", &m_GameLayer->m_FogData[1], 0.0f, 5.f);
		ImGui::SliderFloat3("Fog color", &m_GameLayer->m_FogColor[0], 0.0f, 1.f);
	}
	ImGui::End();
}

int id = 0;

void DrawShader(const char* name, Ref<Shader> shader)
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
		for (auto& shader : m_GameLayer->m_ShaderLibrary)
		{
			DrawShader(shader.first.c_str(), shader.second);
		}
	}
	ImGui::End();
}

void GameLayerImGui::DrawImage(uint32_t textureId, float my_tex_w, float my_tex_h)
{
	const ImGuiIO& io = ImGui::GetIO();
	const ImTextureID my_tex_id = (ImTextureID)textureId;
	{
		const ImVec2 pos = ImGui::GetCursorScreenPos();
		const ImVec2 uv_min = ImVec2(0.0f, 1.0f); // Top-left
		const ImVec2 uv_max = ImVec2(1.0f, 0.0f); // Lower-right
		ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			const float region_sz = 32.0f;
			float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
			float region_y = my_tex_h - (io.MousePos.y - pos.y);
			region_y = my_tex_h - region_y;
			region_y = region_y - region_sz * 0.5f;
			const float zoom = 7.0f;
			if (region_x < 0.0f) { region_x = 0.0f; }
			else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
			if (region_y < 0.0f) { region_y = 0.0f; }
			else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }
			ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
			ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
			const ImVec2 uv0((region_x) / my_tex_w, (region_y + region_sz) / my_tex_h);
			const ImVec2 uv1((region_x + region_sz) / my_tex_w, (region_y) / my_tex_h);
			ImGui::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1);
			ImGui::EndTooltip();
		}
	}
}

static int gameObjectPropsId = 0;
static int selectionMask = -1;

int DrawVectorComponent(const char* name, float* num, const ImVec4& color, const float defaultValue = 0.0f)
{
	int changed = 0;

	ImGui::PushStyleColor(ImGuiCol_Button, color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(25.0f, 0.0f));
	ImGui::PushID(gameObjectPropsId++);
	if (ImGui::Button(name, ImVec2(20, 0)))
	{
		changed |= 1;
		*num = defaultValue;
	}
	ImGui::PopID();
	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();
	ImGui::SameLine();
	ImGui::PushID(gameObjectPropsId++);
	ImGui::SetNextItemWidth(60.0f);
	changed |= ImGui::DragFloat("", num, 0.15f, -FLT_MAX, FLT_MAX, "%.2f");
	ImGui::PopID();

	return changed;
}

int DrawVector(glm::vec3& vector, const float defaultValue = 0.0f)
{
	static ImVec4 xColor = ImVec4(216.0f / 255.0f, 27.0f / 255.0f, 5.0f / 255.0f, 1);
	static ImVec4 yColor = ImVec4(34.0f / 255.0f, 177.0f / 255.0f, 76.0f / 255.0f, 1);
	static ImVec4 zColor = ImVec4(2.0f / 255.0f, 68.0f / 255.0f, 168.0f / 255.0f, 1);

	int dirty = 0;

	dirty |= DrawVectorComponent("X", &vector[0], xColor, defaultValue);
	ImGui::SameLine();
	dirty |= DrawVectorComponent("Y", &vector[1], yColor, defaultValue);
	ImGui::SameLine();
	dirty |= DrawVectorComponent("Z", &vector[2], zColor, defaultValue);

	return dirty;
}

void GameLayerImGui::PropertiesPanel()
{
	if (ImGui::Begin("Properties"))
	{
		gameObjectPropsId = 0;
		if (!m_SelectedObject)
		{
			ImGui::End();
			return;
		}

		// Transform
		{
			ImGui::Spacing();
			ImGui::Text("Transform");
			ImGui::Spacing();
			ImGui::Spacing();

			auto& position = m_SelectedObject->GetPosition();
			ImGui::Text("Position");
			ImGui::SameLine();
			if (DrawVector(position))
				m_SelectedObject->SetPosition(position);

			auto rotation = glm::degrees(m_SelectedObject->GetRotation());
			ImGui::Text("Rotation");
			ImGui::SameLine();
			if (DrawVector(rotation))
			{
				rotation = glm::radians(rotation);
				m_SelectedObject->SetRotation(rotation);
			}

			auto& scale = m_SelectedObject->GetScale();
			ImGui::Text("Scale   ");
			ImGui::SameLine();
			if (DrawVector(scale, 1.0f))
				m_SelectedObject->SetScale(scale);
		
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
		}
	
		{
			ImGui::Spacing();
			ImGui::Text("Material");
			ImGui::Spacing();
			ImGui::Spacing();
			m_SelectedObject->GetMaterial()->DrawImGui();
		}
	}
	ImGui::End();
}

void GameLayerImGui::GameObjectsPanel()
{

	if (ImGui::Begin("Game objects"))
	{
		static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		int nodeClicked = -1;
		int i = 0;
		for (auto& element : m_GameLayer->m_GameObjects)
		{
			ImGuiTreeNodeFlags nodeFlags = baseFlags;
			const bool isSelected = selectionMask == -1 ? false : (selectionMask & (1 << i)) != 0;
			if (isSelected)
			{
				nodeFlags |= ImGuiTreeNodeFlags_Selected;
				m_SelectedObject = element.second.get();
			}

			bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, element.first.c_str());
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
				nodeClicked = i;

			if (nodeOpen)
			{
				// Child gameObjects
				ImGui::TreePop();
			}
			i++;
		}
		if (nodeClicked != -1)
		{
			if (selectionMask == (1 << nodeClicked)) // De-selected
			{
				selectionMask = -1;
				m_SelectedObject = nullptr;
			}
			else
			{
				selectionMask = (1 << nodeClicked); // Selected
			}
		}
	}
	ImGui::End();
}
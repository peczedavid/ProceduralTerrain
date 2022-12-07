#include "pch.h"

#include "ClothSimulationLayerImGui.h"
#include <Core/Application.h>
#include <ImGuizmo.h>
#include <Core/Math/Math.h>
#include <dxgi1_4.h>
#include <Rendering/Renderer.h>

ClothSimulationLayerImGui::ClothSimulationLayerImGui(ClothSimulationLayer* layer)
	: m_Layer(layer)
{
}

void ClothSimulationLayerImGui::ViewportPanel()
{
	if (ImGui::Begin("Viewport"))
	{
		Application::Get().SetViewportHovered(ImGui::IsWindowHovered());
		const ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		const float my_tex_w = viewportPanelSize.x;
		const float my_tex_h = viewportPanelSize.y;
		m_Layer->m_ActiveCamera->Resize(my_tex_w, my_tex_h);
		if (m_Layer->m_ViewportSize.x != viewportPanelSize.x || m_Layer->m_ViewportSize.y != viewportPanelSize.y)
		{
			m_Layer->m_FrameBuffer->Resize(my_tex_w, my_tex_h);
			m_Layer->m_ViewportSize.x = viewportPanelSize.x;
			m_Layer->m_ViewportSize.y = viewportPanelSize.y;
		}
		else
		{
			const ImVec2 uv_min(0.0f, 1.0f); // Top-left
			const ImVec2 uv_max(1.0f, 0.0f); // Lower-right
			ImGui::Image((ImTextureID)m_Layer->m_FrameBuffer->GetTextureId(), ImVec2(my_tex_w, my_tex_h), uv_min, uv_max);

			if (m_SelectedObject)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				auto camera = m_Layer->m_ActiveCamera;
				const glm::mat4 view = camera->GetView();
				const glm::mat4 proj = camera->GetProj();
				glm::mat4& transform = m_SelectedObject->GetTransform();

				static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
				static ImGuizmo::MODE mode = ImGuizmo::MODE::LOCAL;

				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
				{
					if (ImGui::IsKeyDown(ImGuiKey_W))
					{
						operation = ImGuizmo::OPERATION::TRANSLATE;
						mode = ImGuizmo::MODE::WORLD;
					}
					else if (ImGui::IsKeyDown(ImGuiKey_E))
					{
						operation = ImGuizmo::OPERATION::ROTATE;
						mode = ImGuizmo::MODE::LOCAL;
					}
					else if (ImGui::IsKeyDown(ImGuiKey_R))
					{
						operation = ImGuizmo::OPERATION::SCALE;
						mode = ImGuizmo::MODE::LOCAL;
					}
				}

				ImGuizmo::Manipulate(&view[0][0], &proj[0][0], operation, mode, &transform[0][0]);

				if (ImGuizmo::IsUsing())
				{
					glm::vec3 translation, rotation, scale;
					DecomposeTransform(transform, translation, rotation, scale);
					m_SelectedObject->Set(translation, rotation, scale);
				}
			}
		}
	}
	ImGui::End();
}

void ClothSimulationLayerImGui::ControlsPanel()
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
		ImGui::Text("Ctrl+G - Grid");
	}
	ImGui::End();
}

void ClothSimulationLayerImGui::VendorInfoPanel()
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
		MEMORYSTATUSEX memInfo{};
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		DWORDLONG totalPhysMem = memInfo.ullTotalPhys / 1024 / 1024;
		DWORDLONG physMemUsed = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / 1024 / 1024;
		ImGui::Text("RAM: %llu/%llu MB", physMemUsed, totalPhysMem);
	}
	ImGui::End();
}

static int gameObjectPropsId = 0;
static int selectionMask = -1;

void ClothSimulationLayerImGui::GameObjectsPanel()
{
	if (ImGui::Begin("Game objects"))
	{
		static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_SpanAvailWidth;

		int nodeClicked = -1;
		int i = 0;
		for (auto& element : m_Layer->m_GameObjects)
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

int DrawVecComponent(const char* name, float* num, const ImVec4& color, const float defaultValue = 0.0f)
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

int DrawVec(glm::vec3& vector, const float defaultValue = 0.0f)
{
	static ImVec4 xColor = ImVec4(216.0f / 255.0f, 27.0f / 255.0f, 5.0f / 255.0f, 1);
	static ImVec4 yColor = ImVec4(34.0f / 255.0f, 177.0f / 255.0f, 76.0f / 255.0f, 1);
	static ImVec4 zColor = ImVec4(2.0f / 255.0f, 68.0f / 255.0f, 168.0f / 255.0f, 1);

	int dirty = 0;

	dirty |= DrawVecComponent("X", &vector[0], xColor, defaultValue);
	ImGui::SameLine();
	dirty |= DrawVecComponent("Y", &vector[1], yColor, defaultValue);
	ImGui::SameLine();
	dirty |= DrawVecComponent("Z", &vector[2], zColor, defaultValue);

	return dirty;
}

void ClothSimulationLayerImGui::PropertiesPanel()
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
			if (DrawVec(position))
				m_SelectedObject->SetPosition(position);

			auto rotation = glm::degrees(m_SelectedObject->GetRotation());
			ImGui::Text("Rotation");
			ImGui::SameLine();
			if (DrawVec(rotation))
			{
				rotation = glm::radians(rotation);
				m_SelectedObject->SetRotation(rotation);
			}

			auto& scale = m_SelectedObject->GetScale();
			ImGui::Text("Scale   ");
			ImGui::SameLine();
			if (DrawVec(scale, 1.0f))
				m_SelectedObject->SetScale(scale);

			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Separator();
		}

		if (m_SelectedObject->GetMaterial())
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

void ClothSimulationLayerImGui::ShadersPanel()
{
	if (ImGui::Begin("Shaders"))
	{
		static int s_ShaderId = 0;
		for (auto& shader : m_Layer->m_ShaderLibrary)
		{
			ImGui::Text(shader.first.c_str());
			ImGui::SameLine();
			ImGui::PushID(s_ShaderId++);
			if (ImGui::Button("Reload"))
				shader.second->ReCompile();
			ImGui::PopID();
		}
	}
	ImGui::End();
}

void ClothSimulationLayerImGui::GraphicsSettingsPanel()
{
	if (ImGui::Begin("Graphics settings"))
	{
		auto window = Application::Get().GetWindow();
		bool vSync = window->IsVSync();
		ImGui::Checkbox("VSync", &vSync);
		window->SetVSync(vSync);
		if (ImGui::Button("Toggle camera"))
		{
			m_Layer->ToggleCamera();
		}
	}
	ImGui::End();
}

void ClothSimulationLayerImGui::DebugOverlayPanel()
{
	const ImGuiIO& io = ImGui::GetIO();
	const ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("Debug overaly", (bool*)1, window_flags);
	ImGui::Text("Viewport size: %.0fx%.0f", m_Layer->m_ViewportSize.x, m_Layer->m_ViewportSize.y);
	const glm::vec3 cameraPos = m_Layer->m_ActiveCamera->GetPosition();
	ImGui::Text("XYZ: %.2f / %.2f / %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
	const glm::vec3 cameraOri = m_Layer->m_ActiveCamera->GetOrientation();
	ImGui::Text("Facing: %.2f / %.2f / %.2f", cameraOri.x, cameraOri.y, cameraOri.z);
	ImGui::Text("%d FPS (max: %.0f)", m_Layer->m_FPS, Renderer::FPSPool.GetMax());
	ImGui::Text("%.4f ms", 1.0f / m_Layer->m_FPS);
	const auto& samples = Renderer::FPSPool.GetSamples();
	ImGui::PlotLines("FPS", &samples[0], samples.size(), 0, 0, 0, Renderer::FPSPool.GetMax(), ImVec2(175, 40));
	ImGui::End();
	ImGui::PopStyleVar();
}
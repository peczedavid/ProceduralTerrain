#pragma once

#include "ClothSimulationLayer.h"

class ClothSimulationLayerImGui
{
public:
	ClothSimulationLayerImGui(ClothSimulationLayer* layer);

	void ViewportPanel();
	void ControlsPanel();
	void VendorInfoPanel();
	void GameObjectsPanel();
	void PropertiesPanel();
	void ShadersPanel();
	void GraphicsSettingsPanel();
	void DebugOverlayPanel();
public:
	static Ref<Texture2D> s_PlaceHolderTexture;
private:
	ClothSimulationLayer* m_Layer;
	GameObject* m_SelectedObject = nullptr;
};
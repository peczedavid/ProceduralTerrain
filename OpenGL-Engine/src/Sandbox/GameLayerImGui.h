#pragma once

#include "Sandbox/GameLayer.h"

class GameLayerImGui
{
public:
	GameLayerImGui(GameLayer* gameLayer);

	void ViewportPanel();
	void NoisePanel();
	void LandscapePanel();
	void WaterPanel();
	void ContolsPanel();
	void VendorInfoPanel();
	void DebugOverlayPanel();
	void TexturesPanel();
	void GraphicsSettingsPanel();
	void PropertiesPanel();
	void GameObjectsPanel();
	void EnviromentPanel();
	void ShadersPanel();
public:
	static Ref<Texture2D> s_PlaceHolderTexture;
private:
	void DrawImage(uint32_t textureId, float my_tex_w = 200.0f, float my_tex_h = 200.0f);
private:
	GameLayer* m_GameLayer;
	GameObject* m_SelectedObject = nullptr;
};
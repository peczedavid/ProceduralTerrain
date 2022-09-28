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
	void EnviromentPanel();
	void ShadersPanel();
private:
	void DrawImage(uint32_t textureId, float my_tex_w = 200.0f, float my_tex_h = 200.0f);
private:
	GameLayer* m_GameLayer;
};
#pragma once

#include "Core/Layers/Layer.h"

class ImGuiLayer : public Layer
{
public:
	ImGuiLayer();
	~ImGuiLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnImGuiRender(float dt) override;

	void Begin();
	void End();

	static void SetCustomStyle();
private:
	void SetBlackTheme();
	void SetStyle();
private:
	float m_Time = 0.0f;
};

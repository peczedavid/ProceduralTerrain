#pragma once

#include <string>

class Layer
{
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer();

	virtual void OnAttach() { }
	virtual void OnDetach() { }
	virtual void OnUpdate(float dt) { }
	virtual void OnImGuiRender(float dt) { }

	virtual void OnResize(uint32_t width, uint32_t height) { }
	virtual void OnScreenshot() { }

	inline const std::string& GetName() const { return m_Name; }
protected:
	std::string m_Name;
};
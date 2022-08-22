#pragma once

#include "Core/Layers/Layer.h"
#include <vector>

// - Wrapper class for std::vector<Layer*> m_Layers
// - Vector, beacuse every frame it needs to be iterated through

// - LAYERS get pushed into the FIRST HALF
// - OVERLAYS get pushed into the SECOND HALF
class LayerStack
{
	using LayerIterator = std::vector<Layer*>::iterator;
public:
	LayerStack();
	~LayerStack();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	void PopLayer(Layer* layer);
	void PopOverlay(Layer* overlay);

	LayerIterator begin() { return m_Layers.begin(); }
	LayerIterator end() { return m_Layers.end(); }
private:
	std::vector<Layer*> m_Layers;

	uint32_t m_LayerInsertIndex = 0;
};
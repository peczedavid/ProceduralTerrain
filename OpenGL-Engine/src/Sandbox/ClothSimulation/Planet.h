#pragma once

#include "Rendering/GameObject.h"

struct PlanetData
{
	float Mass = 1.0f;
	float Radius = 1.0f;
	glm::vec3 Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
};

class Planet : public GameObject
{
public:
	Planet(Model* model, Ref<Material> material, const PlanetData& m_PlanetData = PlanetData());

	void UpdateVelocity(const std::vector<Ref<Planet>>& planets, float dt);
	void UpdatePosition(float dt);
private:
	PlanetData m_PlanetData;
};

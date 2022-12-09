#include "pch.h"

#include "Planet.h"

Planet::Planet(Model* model, Ref<Material> material, const PlanetData& planetData)
	: GameObject(model, material), m_PlanetData(planetData)
{
}

void Planet::UpdateVelocity(const std::vector<Ref<Planet>>& planets, float dt)
{
	for (auto& other : planets)
	{
		if (other.get() != this)
		{
			const float r = glm::length(other->GetPosition() - GetPosition());
			const float sqrtDist = r * r;
			glm::vec3 forceDir = glm::normalize(other->GetPosition() - GetPosition());
			glm::vec3 force = forceDir * 0.64f * m_PlanetData.Mass * other->m_PlanetData.Mass / sqrtDist;
			glm::vec3 acc = force / m_PlanetData.Mass;
			m_PlanetData.Velocity += acc * dt;
		}
	}
}

void Planet::UpdatePosition(float dt)
{
	SetPosition(GetPosition() + m_PlanetData.Velocity * dt);
}

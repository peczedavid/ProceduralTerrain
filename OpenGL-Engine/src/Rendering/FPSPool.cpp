#include "Rendering/FPSPool.h"

FPSPool::FPSPool()
{
	m_Pool.resize(m_PoolSize);
}

void FPSPool::AddFPSSample(float FPS)
{
	for (size_t i = 0; i < m_PoolSize - 1; i++)
	{
		m_Pool.at(i) = m_Pool.at(i + 1);
		m_Pool[m_PoolSize - 1] = FPS;
	}
}

const std::vector<float>& FPSPool::GetSamples() const
{
	return m_Pool;
}

const float FPSPool::GetMax() const
{
	return m_Max;
}

void FPSPool::SetMax(float maxFPS)
{
	m_Max = maxFPS;
}

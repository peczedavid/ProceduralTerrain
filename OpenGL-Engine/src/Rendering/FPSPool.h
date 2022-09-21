#pragma once
#include <cinttypes>
#include <vector>

class FPSPool
{
public:
	FPSPool();
	void AddFPSSample(float FPS);

	const std::vector<float>& GetSamples() const;
	const float GetMax() const;
	void SetMax(float maxFPS);
private:
	const size_t m_PoolSize = 600; // Last 10 seconds of samples
	std::vector<float> m_Pool;
	size_t m_PoolIndex = 0;
	float m_Max = 0;
};
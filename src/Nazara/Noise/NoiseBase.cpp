// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/NoiseBase.hpp>
#include <numeric>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	NoiseBase::NoiseBase(unsigned int seed) :
	m_scale(0.05f)
	{
		SetSeed(seed);

		// Fill permutations with initial values
		std::iota(m_permutations.begin(), m_permutations.begin() + 256, 0);
	}

	float NoiseBase::GetScale()
	{
		return m_scale;
	}

	void NoiseBase::SetScale(float scale)
	{
		m_scale = scale;
	}

	void NoiseBase::SetSeed(unsigned int seed)
	{
		m_randomEngine.seed(seed);
	}

	void NoiseBase::Shuffle()
	{
		std::shuffle(m_permutations.begin(), m_permutations.begin() + 256, m_randomEngine);

		for(std::size_t i = 1; i < (m_permutations.size() / 256); ++i)
			std::copy(m_permutations.begin(), m_permutations.begin() + 256, m_permutations.begin() + 256 * i);
	}

	std::array<Vector2f, 2 * 2 * 2> NoiseBase::s_gradients2 =
	{
		{
			{1.f, 1.f}, {-1.f, 1.f}, {1.f, -1.f}, {-1.f, -1.f},
			{1.f, 0.f}, {-1.f, 0.f}, {0.f,  1.f}, { 0.f, -1.f}
		}
	};

	std::array<Vector3f, 2 * 2 * 2 * 2> NoiseBase::s_gradients3 =
	{
		{
			{1.f,1.f,0.f}, {-1.f,  1.f, 0.f}, {1.f, -1.f,  0.f}, {-1.f, -1.f,  0.f},
			{1.f,0.f,1.f}, {-1.f,  0.f, 1.f}, {1.f,  0.f, -1.f}, {-1.f,  0.f, -1.f},
			{0.f,1.f,1.f}, { 0.f, -1.f, 1.f}, {0.f,  1.f, -1.f}, {0.f,  -1.f, -1.f},
			{1.f,1.f,0.f}, {-1.f,  1.f, 0.f}, {0.f, -1.f,  1.f}, {0.f,  -1.f, -1.f}
		}
	};

	std::array<Vector4f, 2 * 2 * 2 * 2 * 2> NoiseBase::s_gradients4 =
	{
		{
			{0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1},
			{0,-1,1,1},{0,-1,1,-1},{0,-1,-1,1},{0,-1,-1,-1},
			{1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
			{-1,0,1,1},{-1,0,1,-1},{-1,0,-1,1},{-1,0,-1,-1},
			{1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
			{-1,1,0,1},{-1,1,0,-1},{-1,-1,0,1},{-1,-1,0,-1},
			{1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
			{-1,1,1,0},{-1,1,-1,0},{-1,-1,1,0},{-1,-1,-1,0}
		}
	};
}

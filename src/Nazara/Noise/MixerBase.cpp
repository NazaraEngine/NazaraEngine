// Copyright (C) 2017 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/MixerBase.hpp>
#include <cmath>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	MixerBase::MixerBase() :
	m_hurst(1.2f),
	m_lacunarity(5.f),
	m_octaves(3.f)
	{
		Recompute();
	}

	float MixerBase::GetHurstParameter() const
	{
		return m_hurst;
	}

	float MixerBase::GetLacunarity() const
	{
		return m_lacunarity;
	}

	float MixerBase::GetOctaveNumber() const
	{
		return m_octaves;
	}

	void MixerBase::SetParameters(float hurst, float lacunarity, float octaves)
	{
		m_hurst = hurst;
		m_lacunarity = lacunarity;
		m_octaves = octaves;

		Recompute();
	}

	void MixerBase::Recompute()
	{
		float frequency = 1.0;
		m_sum = 0.f;
		m_exponent_array.clear();

		for (int i(0) ; i < static_cast<int>(m_octaves) ; ++i)
		{
			m_exponent_array.push_back(std::pow( frequency, -m_hurst ));
			frequency *= m_lacunarity;
			m_sum += m_exponent_array.at(i);
		}
	}
}

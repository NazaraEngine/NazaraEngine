// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/HybridMultiFractal.hpp>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	HybridMultiFractal::HybridMultiFractal(const NoiseBase & source) :
	m_source(source)
	{
	}

	float HybridMultiFractal::Get(std::initializer_list<float> coordinates, float scale) const
	{
		float offset = 1.0f;
		float value = (m_source.Get(coordinates,scale) + offset) * m_exponent_array.at(0);
		float weight = value;
		float signal = 0.f;

		scale *= m_lacunarity;

		for(int i(1) ; i < m_octaves; ++i)
		{
			if (weight > 1.f)
				weight = 1.f;

			signal = (m_source.Get(coordinates,scale) + offset) * m_exponent_array.at(i);
			value += weight * signal;
			weight *= signal;
			scale *= m_lacunarity;
		}

		float remainder = m_octaves - static_cast<int>(m_octaves);

		if (remainder > 0.f)
			value += remainder * m_source.Get(coordinates,scale) * m_exponent_array.at(static_cast<int>(m_octaves-1));

		return value / m_sum - offset;
	}
}

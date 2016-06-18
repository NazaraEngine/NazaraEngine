// Copyright (C) 2016 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/FBM.hpp>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	FBM::FBM(const NoiseBase & source): m_source(source)
	{
	}

	float FBM::Get(std::initializer_list<float> coordinates, float scale) const
	{
		float value = 0.0;

		for(int i(0); i < m_octaves; ++i)
		{
			value += m_source.Get(coordinates,scale) * m_exponent_array.at(i);
			scale *= m_lacunarity;
		}

		float remainder = m_octaves - static_cast<int>(m_octaves);

		if(std::fabs(remainder) > 0.01f)
		  value += remainder * m_source.Get(coordinates,scale) * m_exponent_array.at(static_cast<int>(m_octaves-1));

		return value / m_sum;
	}
}

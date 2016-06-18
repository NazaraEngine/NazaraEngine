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

	///TODO: Handle with variadic templates
	float FBM::Get(float x, float y, float scale) const
	{
		float value = 0.f;
		for(int i = 0; i < m_octaves; ++i)
		{
			value += m_source.Get(x, y, scale) * m_exponent_array.at(i);
			scale *= m_lacunarity;
		}

		float remainder = m_octaves - static_cast<int>(m_octaves);
		if(std::fabs(remainder) > 0.01f)
		  value += remainder * m_source.Get(x, y, scale) * m_exponent_array.at(static_cast<int>(m_octaves-1));

		return value / m_sum;
	}

	float FBM::Get(float x, float y, float z, float scale) const
	{
		float value = 0.f;
		for(int i = 0; i < m_octaves; ++i)
		{
			value += m_source.Get(x, y, z, scale) * m_exponent_array.at(i);
			scale *= m_lacunarity;
		}

		float remainder = m_octaves - static_cast<int>(m_octaves);
		if(std::fabs(remainder) > 0.01f)
		  value += remainder * m_source.Get(x, y, z, scale) * m_exponent_array.at(static_cast<int>(m_octaves-1));

		return value / m_sum;
	}

	float FBM::Get(float x, float y, float z, float w, float scale) const
	{
		float value = 0.f;
		for(int i = 0; i < m_octaves; ++i)
		{
			value += m_source.Get(x, y, z, w, scale) * m_exponent_array.at(i);
			scale *= m_lacunarity;
		}

		float remainder = m_octaves - static_cast<int>(m_octaves);
		if(std::fabs(remainder) > 0.01f)
		  value += remainder * m_source.Get(x, y, z, w, scale) * m_exponent_array.at(static_cast<int>(m_octaves-1));

		return value / m_sum;
	}
}

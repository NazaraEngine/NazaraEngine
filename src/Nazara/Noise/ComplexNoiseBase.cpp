// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cmath>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Debug.hpp>

NzComplexNoiseBase::NzComplexNoiseBase()
{
    m_parametersModified = true;
    m_lacunarity = 5.0f;
    m_hurst = 1.2f;
    m_octaves = 3.0f;

    for (int i(0) ; i < m_octaves; ++i)
    {
        m_exponent_array[i] = 0;
    }
}

float NzComplexNoiseBase::GetLacunarity() const
{

    return m_lacunarity;
}

float NzComplexNoiseBase::GetHurstParameter() const
{
    return m_hurst;
}

float NzComplexNoiseBase::GetOctaveNumber() const
{
    return m_octaves;
}

void NzComplexNoiseBase::SetLacunarity(float lacunarity)
{
    m_lacunarity = lacunarity;
    m_parametersModified = true;

}

void NzComplexNoiseBase::SetHurstParameter(float h)
{
    m_hurst = h;
    m_parametersModified = true;
}

void NzComplexNoiseBase::SetOctavesNumber(float octaves)
{
    if(octaves <= 30.0f)
        m_octaves = octaves;
    else
        m_octaves = 30.0f;

    m_parametersModified = true;
}

void NzComplexNoiseBase::RecomputeExponentArray()
{
    if(m_parametersModified)
    {
        float frequency = 1.0;
        m_sum = 0.f;
        for (int i(0) ; i < static_cast<int>(m_octaves) ; ++i)
        {

            m_exponent_array[i] = std::pow( frequency, -m_hurst );
            frequency *= m_lacunarity;

            m_sum += m_exponent_array[i];

        }
        m_parametersModified = false;
    }
}

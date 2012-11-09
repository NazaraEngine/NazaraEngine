// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cmath>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>

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

const std::array<float, 30>& NzComplexNoiseBase::GetExponentArray() const
{
    return m_exponent_array;
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
   // if(lacunarity != m_lacunarity)
    //{
        m_lacunarity = lacunarity;
        m_parametersModified = true;
    //}
}

void NzComplexNoiseBase::SetHurstParameter(float h)
{
    //if(h != m_hurst)
    //{
        m_hurst = h;
        m_parametersModified = true;
    //}
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

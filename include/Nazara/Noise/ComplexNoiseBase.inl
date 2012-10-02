// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cmath>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

#include <iostream>
using namespace std;

template <typename T>
NzComplexNoiseBase<T>::NzComplexNoiseBase()
{
    m_parametersModified = true;
    m_lacunarity = 5.0f;
    m_hurst = 1.2f;
    m_octaves = 3.0f;

    for (int i(0) ; i < m_octaves; ++i)
    {
        exponent_array[i] = 0;
    }
}

template <typename T>
T NzComplexNoiseBase<T>::GetLacunarity() const
{

    return m_lacunarity;
}

template <typename T>
T NzComplexNoiseBase<T>::GetHurstParameter() const
{
    return m_hurst;
}

template <typename T>
T NzComplexNoiseBase<T>::GetOctaveNumber() const
{
    return m_octaves;
}

template <typename T>
void NzComplexNoiseBase<T>::SetLacunarity(T lacunarity)
{
   // if(lacunarity != m_lacunarity)
    //{
        m_lacunarity = lacunarity;
        m_parametersModified = true;
    //}
}

template <typename T>
void NzComplexNoiseBase<T>::SetHurstParameter(T h)
{
    //if(h != m_hurst)
    //{
        m_hurst = h;
        m_parametersModified = true;
    //}
}

template <typename T>
void NzComplexNoiseBase<T>::SetOctavesNumber(T octaves)
{
    if(octaves <= 30.0f)
        m_octaves = octaves;
    else
        m_octaves = 30.0f;

    m_parametersModified = true;

}

template <typename T>
void NzComplexNoiseBase<T>::RecomputeExponentArray()
{
    if(m_parametersModified)
    {
        cout<<"Recomputing exponent array"<<endl;
        float frequency = 1.0;
        m_sum = 0.f;
        for (int i(0) ; i < static_cast<int>(m_octaves) ; ++i)
        {

            exponent_array[i] = std::pow( frequency, -m_hurst );
            cout<<"expo["<<i<<"] : "<<exponent_array[i]<<endl;
            frequency *= m_lacunarity;

            //m_sum += 1.0f/exponent_array[i];//A tester
            m_sum += exponent_array[i];

        }
        cout<<"sum = "<<m_sum<<endl;
        m_parametersModified = false;
    }
}

#include <Nazara/Core/DebugOff.hpp>

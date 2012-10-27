// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/FBM2D.hpp>
#include <Nazara/Noise/Perlin2D.hpp>
#include <Nazara/Noise/Simplex2D.hpp>
#include <Nazara/Noise/Debug.hpp>

NzFBM2D::NzFBM2D(nzNoises source, int seed)
{
    switch(source)
    {
        case PERLIN:
            m_source = new NzPerlin2D();
        break;

        default:
            m_source = new NzSimplex2D();
        break;
    }
    m_source->SetNewSeed(seed);
    m_source->ShufflePermutationTable();
    m_noiseType = source;
}

float NzFBM2D::GetValue(float x, float y, float resolution)
{
    this->RecomputeExponentArray();

    m_value = 0.0;

    for (int i(0); i < this->m_octaves; ++i)
    {
        m_value += m_source->GetValue(x,y,resolution) * this->exponent_array[i];
        resolution *= this->m_lacunarity;
    }
    //cout<<m_value<<endl;//"|"<<this->m_sum<<endl;
    //m_remainder = this->m_octaves - static_cast<int>(this->m_octaves);

    //if(!NzNumberEquals(remainder, static_cast<float>(0.0)))
      //  m_value += remainder * Get2DSimplexNoiseValue(x,y,resolution) * exponent_array[(int)m_octaves-1];

    //0.65 is an experimental value to make the noise stick closer to [-1 , 1]
    return m_value / (this->m_sum * 0.65);
}

NzFBM2D::~NzFBM2D()
{
    switch(m_noiseType)
    {
        case PERLIN:
            delete dynamic_cast<NzPerlin2D*>(m_source);
        break;

        default:
            delete dynamic_cast<NzSimplex2D*>(m_source);
        break;
    }
}

// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/FBM3D.hpp>
#include <Nazara/Noise/Perlin3D.hpp>
#include <Nazara/Noise/Simplex3D.hpp>
#include <Nazara/Noise/Debug.hpp>

NzFBM3D::NzFBM3D(nzNoises source, unsigned int seed)
{
    switch(source)
    {
        case PERLIN:
            m_source = new NzPerlin3D();
        break;

        default:
            m_source = new NzSimplex3D();
        break;
    }
    m_source->SetNewSeed(seed);
    m_source->ShufflePermutationTable();
    m_noiseType = source;
}

float NzFBM3D::GetValue(float x, float y, float z, float resolution)
{
    this->RecomputeExponentArray();

    m_value = 0.0;

    for (int i(0); i < m_octaves; ++i)
    {
        m_value += m_source->GetValue(x,y,z,resolution) * m_exponent_array[i];
        resolution *= m_lacunarity;
    }
    m_remainder = m_octaves - static_cast<int>(m_octaves);

    if(!NzNumberEquals(m_remainder, static_cast<float>(0.0)))
        m_value += m_remainder * m_source->GetValue(x,y,z,resolution) * m_exponent_array[static_cast<int>(m_octaves-1)];

    return m_value/this->m_sum;
}

NzFBM3D::~NzFBM3D()
{
    delete m_source;
}

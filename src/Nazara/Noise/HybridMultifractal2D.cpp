// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/HybridMultiFractal2D.hpp>
#include <Nazara/Noise/Perlin2D.hpp>
#include <Nazara/Noise/Simplex2D.hpp>
#include <Nazara/Noise/Debug.hpp>

NzHybridMultiFractal2D::NzHybridMultiFractal2D(nzNoises source, int seed)
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

float NzHybridMultiFractal2D::GetValue(float x, float y, float resolution)
{
    this->RecomputeExponentArray();

    float offset = 1.0f;

    m_value = (m_source->GetValue(x,y,resolution) + offset) * m_exponent_array[0];
    float weight = m_value;
    float signal;

    resolution *= m_lacunarity;

    for(int i(1) ; i < m_octaves; ++i)
    {
        if(weight > 1.0)
            weight = 1.0;

        signal = (m_source->GetValue(x,y,resolution) + offset) * m_exponent_array[i];
        m_value += weight * signal;

        weight *= signal;

        resolution *= m_lacunarity;
    }

    m_remainder = m_octaves - static_cast<int>(m_octaves);

    if(remainder != 0)
        m_value += m_remainder * m_source->GetValue(x,y,resolution) * m_exponent_array[static_cast<int>(m_octaves-1)];

    return m_value/this->m_sum;
}

NzHybridMultiFractal2D::~NzHybridMultiFractal2D()
{
    delete m_source;
}

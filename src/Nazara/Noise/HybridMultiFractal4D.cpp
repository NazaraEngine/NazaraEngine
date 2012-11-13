// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/HybridMultiFractal4D.hpp>
#include <Nazara/Noise/Perlin4D.hpp>
#include <Nazara/Noise/Simplex4D.hpp>
#include <Nazara/Noise/Debug.hpp>

NzHybridMultiFractal4D::NzHybridMultiFractal4D(nzNoises source, int seed)
{
    switch(source)
    {
        case PERLIN:
            m_source = new NzPerlin4D();
        break;

        default:
            m_source = new NzSimplex4D();
        break;
    }
    m_source->SetNewSeed(seed);
    m_source->ShufflePermutationTable();
    m_noiseType = source;
}

float NzHybridMultiFractal4D::GetValue(float x, float y, float z, float w, float resolution)
{
    this->RecomputeExponentArray();

    m_offset = 1.0f;

    m_value = (m_source->GetValue(x,y,z,w,resolution) + m_offset) * m_exponent_array[0];
    m_weight = m_value;
    m_signal = 0.f;

    resolution *= m_lacunarity;

    for(int i(1) ; i < m_octaves; ++i)
    {
        if(m_weight > 1.0)
            m_weight = 1.0;

        m_signal = (m_source->GetValue(x,y,z,w,resolution) + m_offset) * m_exponent_array[i];
        m_value += m_weight * m_signal;

        m_weight *= m_signal;

        resolution *= m_lacunarity;
    }

    m_remainder = m_octaves - static_cast<int>(m_octaves);

    if(remainder != 0)
        m_value += m_remainder * m_source->GetValue(x,y,z,w,resolution) * m_exponent_array[static_cast<int>(m_octaves-1)];

    return m_value/this->m_sum - m_offset;
}

NzHybridMultiFractal4D::~NzHybridMultiFractal4D()
{
    delete m_source;
}

// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef HYBRIDMULTIFRACTAL3D_HPP
#define HYBRIDMULTIFRACTAL3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract3DNoise.hpp>

class NAZARA_API NzHybridMultiFractal3D : public NzAbstract3DNoise, public NzComplexNoiseBase
{
    public:
        NzHybridMultiFractal3D(nzNoises source, unsigned int seed);
        float GetValue(float x, float y, float z, float resolution);
        ~NzHybridMultiFractal3D();
    protected:
    private:
        NzAbstract3DNoise* m_source;
        float m_value;
        float m_remainder;
        float m_offset;
        float m_weight;
        float m_signal;
        nzNoises m_noiseType;
};

#endif // HYBRIDMULTIFRACTAL3D_HPP


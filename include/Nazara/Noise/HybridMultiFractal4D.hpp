// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef HYBRIDMULTIFRACTAL4D_HPP
#define HYBRIDMULTIFRACTAL4D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract4DNoise.hpp>

class NAZARA_API NzHybridMultiFractal4D : public NzAbstract4DNoise, public NzComplexNoiseBase
{
    public:
        NzHybridMultiFractal4D(nzNoises source, unsigned int seed);
        float GetValue(float x, float y, float z, float w, float resolution);
        ~NzHybridMultiFractal4D();
    protected:
    private:
        NzAbstract4DNoise* m_source;
        float m_value;
        float m_remainder;
        float m_offset;
        float m_weight;
        float m_signal;
        nzNoises m_noiseType;
};

#endif // HYBRIDMULTIFRACTAL4D_HPP


// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef FBM4D_HPP
#define FBM4D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract4DNoise.hpp>

class NAZARA_API NzFBM4D : public NzAbstract4DNoise, public NzComplexNoiseBase
{
    public:
        NzFBM4D(nzNoises source, unsigned int seed);
        float GetValue(float x, float y, float z, float w, float resolution);
        ~NzFBM4D();
    protected:
    private:
        NzAbstract4DNoise* m_source;
        float m_value;
        float m_remainder;
        nzNoises m_noiseType;
};

#endif // FBM4D_HPP


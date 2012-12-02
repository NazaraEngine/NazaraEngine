// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef FBM2D_HPP
#define FBM2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract2DNoise.hpp>

class NAZARA_API NzFBM2D : public NzAbstract2DNoise, public NzComplexNoiseBase
{
    public:
        NzFBM2D(nzNoises source, unsigned int seed);
        float GetValue(float x, float y, float resolution);
        ~NzFBM2D();
    protected:
    private:
        NzAbstract2DNoise* m_source;
        float m_value;
        float m_remainder;
        nzNoises m_noiseType;
};

#endif // FBM2D_HPP

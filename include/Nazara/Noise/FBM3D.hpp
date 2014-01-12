// Copyright (C) 2014 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef FBM3D_HPP
#define FBM3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract3DNoise.hpp>

class NAZARA_API NzFBM3D : public NzAbstract3DNoise, public NzComplexNoiseBase
{
    public:
        NzFBM3D(nzNoises source, unsigned int seed);
        float GetValue(float x, float y, float z, float resolution);
        ~NzFBM3D();
    protected:
    private:
        NzAbstract3DNoise* m_source;
        float m_value;
        float m_remainder;
        nzNoises m_noiseType;
};

#endif // FBM3D_HPP


// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef FBM2DNOISE_HPP
#define FBM2DNOISE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Noise/Abstract2DNoise.hpp>

template <typename T> class NzFBM2D : public NzAbstract2DNoise<T>, public NzComplexNoiseBase<T>
{
    public:
        NzFBM2D(nzNoises source, int seed);
        T GetValue(T x, T y, T resolution);
        ~NzFBM2D();
    protected:
    private:
        NzAbstract2DNoise<T>* m_source;
        T m_value;
        T m_remainder;
        nzNoises m_noiseType;
};

typedef NzFBM2D<float>  NzFBM2Df;
typedef NzFBM2D<double> NzFBM2Dd;

#include <Nazara/Noise/FBM2D.inl>

#endif // FBM2DNOISE_HPP

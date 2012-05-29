// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PERLIN1D_H
#define PERLIN1D_H

#include <Nazara/Prerequesites.hpp>
//#include <Nazara/Noise/NoiseBase.hpp>
#include "NoiseBase.hpp"

template <typename T> class NzPerlin1D : public NzNoiseBase
{
    public:
        NzPerlin1D();
        T GetValue(T x, T res);
        ~NzPerlin1D() = default;
    protected:
    private:
        int x0;
        int gi0,gi1;
        int ii;
        int gradient1[16];
        T s,t;
        T Cx;
        T nx;
        T tmp;
};

typedef NzPerlin1D<float>  NzPerlin1Df;
typedef NzPerlin1D<double> NzPerlin1Dd;

//#include <Nazara/Noise/Perlin1D.inl>
#include "Perlin1D.inl"

#endif // PERLIN1D_H

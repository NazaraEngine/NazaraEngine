// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PERLIN2D_H
#define PERLIN2D_H

#include <Nazara/Prerequesites.hpp>
//#include <Nazara/Noise/NoiseBase.hpp>
#include "NoiseBase.hpp"
#include <Nazara/Math/Vector2.hpp>

template <typename T> class NzPerlin2D : public NzNoiseBase
{
    public:
        NzPerlin2D();
        T GetValue(T x, T y, T res);
        ~NzPerlin2D() = default;
    protected:
    private:
        int x0, y0;
        int gi0,gi1,gi2,gi3;
        int ii, jj;
        T gradient2[8][2];
        T s,t,u,v;
        T Cx,Cy;
        T Li1, Li2;
        T nx, ny;
        T tmp;
        NzVector2<T> temp;
};

typedef NzPerlin2D<float>  NzPerlin2Df;
typedef NzPerlin2D<double> NzPerlin2Dd;

//#include <Nazara/Noise/Perlin2D.inl>
#include "Perlin2D.inl"

#endif // PERLIN2D_H


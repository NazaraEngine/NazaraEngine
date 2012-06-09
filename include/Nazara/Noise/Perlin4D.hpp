// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PERLIN4D_H
#define PERLIN4D_H

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Math/Vector4.hpp>

template <typename T> class NAZARA_API NzPerlin4D : public NzNoiseBase
{
    public:
        NzPerlin4D();
        T GetValue(T x, T y, T z, T w, T res);
        ~NzPerlin4D() = default;
    protected:
    private:
        int x0,y0,z0,w0;
        int gi0,gi1,gi2,gi3,gi4,gi5,gi6,gi7,gi8,gi9,gi10,gi11,gi12,gi13,gi14,gi15;
        int ii,jj,kk,ll;
        int gradient4[32][4];
        T Li1,Li2,Li3,Li4,Li5,Li6,Li7,Li8,Li9,Li10,Li11,Li12,Li13,Li14;
        T s[4],t[4],u[4],v[4];
        T Cx,Cy,Cz,Cw;
        T nx,ny,nz,nw;
        T tmp;
        NzVector4<T> temp;

};

typedef NzPerlin4D<float>  NzPerlin4Df;
typedef NzPerlin4D<double> NzPerlin4Dd;

#include <Nazara/Noise/Perlin4D.inl>

#endif // PERLIN4D_H

// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PERLIN3D_HPP
#define PERLIN3D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Abstract3DNoise.hpp>
#include <Nazara/Math/Vector3.hpp>

template <typename T> class NzPerlin3D : public NzAbstract3DNoise<T>
{
    public:
        NzPerlin3D();
        T GetValue(T x, T y, T z, T resolution);
        ~NzPerlin3D() = default;
    protected:
    private:
        int x0,y0,z0;
        int gi0,gi1,gi2,gi3,gi4,gi5,gi6,gi7;
        int ii,jj,kk;
        int gradient3[16][3];
        T Li1,Li2,Li3,Li4,Li5,Li6;
        T s[2],t[2],u[2],v[2];
        T Cx,Cy,Cz;
        T nx,ny,nz;
        T tmp;
        NzVector3<T> temp;

};

typedef NzPerlin3D<float>  NzPerlin3Df;
typedef NzPerlin3D<double> NzPerlin3Dd;

#include <Nazara/Noise/Perlin3D.inl>

#endif // PERLIN3D_HPP

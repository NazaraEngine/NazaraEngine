// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SIMPLEX2D_HPP
#define SIMPLEX2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Abstract2DNoise.hpp>
#include <Nazara/Math/Vector2.hpp>

template <typename T> class NAZARA_API NzSimplex2D : public NzAbstract2DNoise<T>, public NzNoiseBase
{
    public:
        NzSimplex2D();
        T GetValue(T x, T y, T resolution);
        ~NzSimplex2D() = default;
    protected:
    private:
        int ii,jj;
        int gi0,gi1,gi2;
        NzVector2i skewedCubeOrigin,off1;
        T n1,n2,n3;
        T c1,c2,c3;
        T gradient2[8][2];
        T UnskewCoeff2D;
        T SkewCoeff2D;
        T sum;
        NzVector2<T> unskewedCubeOrigin, unskewedDistToOrigin;
        NzVector2<T> d1,d2,d3;


};

typedef NzSimplex2D<float>  NzSimplex2Df;
typedef NzSimplex2D<double> NzSimplex2Dd;

#include <Nazara/Noise/Simplex2D.inl>

#endif // SIMPLEX2D_HPP


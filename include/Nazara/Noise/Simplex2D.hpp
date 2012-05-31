// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SIMPLEX2D_H
#define SIMPLEX2D_H

#include <Nazara/Prerequesites.hpp>
//#include <Nazara/Noise/NoiseBase.hpp>
#include "NoiseBase.hpp"
#include <Nazara/Math/Vector2.hpp>


template <typename T> class NzSimplex2D : public NzNoiseBase
{
    public:
        NzSimplex2D();
        T GetValue(T x, T y, T res);
        ~NzSimplex2D() = default;
    protected:
    private:
        int ii,jj;
        int gi0,gi1,gi2;
        NzVector2i Origin,off1;
        T n1,n2,n3;
        T c1,c2,c3;
        T gradient2[8][2];
        T UnskewCoeff2D;
        T SkewCoeff2D;
        NzVector2<T> A, IsoOriginDist;
        NzVector2<T> d1,d2,d3;


};

typedef NzSimplex2D<float>  NzSimplex2Df;
typedef NzSimplex2D<double> NzSimplex2Dd;

//#include <Nazara/Noise/Simplex2D.inl>
#include "Simplex2D.inl"

#endif // SIMPLEX2D_H


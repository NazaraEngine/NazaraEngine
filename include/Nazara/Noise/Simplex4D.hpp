// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SIMPLEX4D_HPP
#define SIMPLEX4D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Abstract4DNoise.hpp>
#include <Nazara/Math/Vector4.hpp>

template <typename T> class NAZARA_API NzSimplex4D : public NzAbstract4DNoise<T>, public NzNoiseBase
{
    public:
        NzSimplex4D();
        T GetValue(T x, T y, T z, T w, T resolution);
        ~NzSimplex4D() = default;
    protected:
    private:
        int ii,jj,kk,ll;
        int gi0,gi1,gi2,gi3,gi4;
        NzVector4i skewedCubeOrigin,off1,off2,off3;
        T n1,n2,n3,n4,n5;
        T c1,c2,c3,c4,c5,c6;
        T gradient4[32][4];
        int lookupTable4D[64][4];
        int c;
        T UnskewCoeff4D;
        T SkewCoeff4D;
        T sum;
        NzVector4<T> unskewedCubeOrigin, unskewedDistToOrigin;
        NzVector4<T> d1,d2,d3,d4,d5;


};

typedef NzSimplex4D<float>  NzSimplex4Df;
typedef NzSimplex4D<double> NzSimplex4Dd;

#include <Nazara/Noise/Simplex4D.inl>

#endif // SIMPLEX4D_H


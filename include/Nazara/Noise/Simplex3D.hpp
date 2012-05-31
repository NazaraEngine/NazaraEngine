// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SIMPLEX3D_H
#define SIMPLEX3D_H

#include <Nazara/Prerequesites.hpp>
//#include <Nazara/Noise/NoiseBase.hpp>
#include "NoiseBase.hpp"
#include <Nazara/Math/Vector3.hpp>


template <typename T> class NzSimplex3D : public NzNoiseBase
{
    public:
        NzSimplex3D();
        T GetValue(T x, T y, T z, T res);
        ~NzSimplex3D() = default;
    protected:
    private:
        int ii,jj,kk;
        int gi0,gi1,gi2,gi3;
        NzVector3i Origin,off1,off2;
        T n1,n2,n3,n4;
        T c1,c2,c3,c4;
        T gradient3[16][3];
        T UnskewCoeff3D;
        T SkewCoeff3D;
        NzVector3<T> A, IsoOriginDist;
        NzVector3<T> d1,d2,d3,d4;


};

typedef NzSimplex3D<float>  NzSimplex3Df;
typedef NzSimplex3D<double> NzSimplex3Dd;

//#include <Nazara/Noise/Simplex3D.inl>
#include "Simplex3D.inl"

#endif // SIMPLEX3D_H


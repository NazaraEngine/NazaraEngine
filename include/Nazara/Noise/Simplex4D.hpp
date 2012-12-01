// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SIMPLEX4D_HPP
#define SIMPLEX4D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Abstract4DNoise.hpp>
#include <Nazara/Math/Vector4.hpp>

class NAZARA_API NzSimplex4D : public NzAbstract4DNoise
{
    public:
        NzSimplex4D();
        NzSimplex4D(unsigned int seed);
        float GetValue(float x, float y, float z, float w, float resolution);
        ~NzSimplex4D() = default;
    protected:
    private:
        int ii,jj,kk,ll;
        int gi0,gi1,gi2,gi3,gi4;
        NzVector4i skewedCubeOrigin,off1,off2,off3;
        int lookupTable4D[64][4];
        int c;
        float n1,n2,n3,n4,n5;
        float c1,c2,c3,c4,c5,c6;
        float gradient4[32][4];
        float UnskewCoeff4D;
        float SkewCoeff4D;
        float sum;
        NzVector4<float> unskewedCubeOrigin, unskewedDistToOrigin;
        NzVector4<float> d1,d2,d3,d4,d5;
};

#endif // SIMPLEX4D_H


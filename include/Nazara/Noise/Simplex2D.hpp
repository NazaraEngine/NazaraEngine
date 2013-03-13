// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef SIMPLEX2D_HPP
#define SIMPLEX2D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Abstract2DNoise.hpp>
#include <Nazara/Math/Vector2.hpp>

class NAZARA_API NzSimplex2D : public NzAbstract2DNoise
{
    public:
        NzSimplex2D();
        NzSimplex2D(unsigned int seed);
        float GetValue(float x, float y, float resolution);
        virtual ~NzSimplex2D() = default;
    protected:
    private:
        int ii,jj;
        int gi0,gi1,gi2;
        NzVector2i skewedCubeOrigin,off1;
        float n1,n2,n3;
        float c1,c2,c3;
        float gradient2[8][2];
        float UnskewCoeff2D;
        float SkewCoeff2D;
        float sum;
        NzVector2<float> unskewedCubeOrigin, unskewedDistToOrigin;
        NzVector2<float> d1,d2,d3;
};

#endif // SIMPLEX2D_HPP


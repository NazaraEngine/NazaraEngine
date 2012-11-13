// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef PERLIN4D_HPP
#define PERLIN4D_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>
#include <Nazara/Noise/Abstract4DNoise.hpp>
#include <Nazara/Math/Vector4.hpp>

class NAZARA_API NzPerlin4D : public NzAbstract4DNoise
{
    public:
        NzPerlin4D();
        NzPerlin4D(int seed);
        float GetValue(float x, float y, float z, float w, float resolution);
        ~NzPerlin4D() = default;
    protected:
    private:
        int x0,y0,z0,w0;
        int gi0,gi1,gi2,gi3,gi4,gi5,gi6,gi7,gi8,gi9,gi10,gi11,gi12,gi13,gi14,gi15;
        int ii,jj,kk,ll;
        float gradient4[32][4];
        float Li1,Li2,Li3,Li4,Li5,Li6,Li7,Li8,Li9,Li10,Li11,Li12,Li13,Li14;
        float s[4],t[4],u[4],v[4];
        float Cx,Cy,Cz,Cw;
        float tmp;
        NzVector4<float> temp;
};

#endif // PERLIN4D_HPP

// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NOISEMACHINE_HPP
#define NOISEMACHINE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/ComplexNoiseBase.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>

class NAZARA_API NzNoiseMachine : public NzComplexNoiseBase
{
    public:
        NzNoiseMachine(int seed = 0);
        ~NzNoiseMachine() = default;

        float Get2DPerlinNoiseValue (float x, float y,                   float res);
        float Get3DPerlinNoiseValue (float x, float y, float z,          float res);
        float Get4DPerlinNoiseValue (float x, float y, float z, float w, float res);

        float Get2DSimplexNoiseValue(float x, float y,                   float res);
        float Get3DSimplexNoiseValue(float x, float y, float z,          float res);
        float Get4DSimplexNoiseValue(float x, float y, float z, float w, float res);

        float Get2DCellNoiseValue(float x, float y,                   float res);
        float Get3DCellNoiseValue(float x, float y, float z,          float res);
        float Get4DCellNoiseValue(float x, float y, float z, float w, float res);

        float Get2DFBMNoiseValue(float x, float y,          float res);
        float Get3DFBMNoiseValue(float x, float y, float z, float res);

        float Get2DHybridMultiFractalNoiseValue(float x, float y,          float res);
        float Get3DHybridMultiFractalNoiseValue(float x, float y, float z, float res);

    protected:
    private:

        float gradient2[8][2];
        int gradient3[16][3];
        int gradient4[32][4];
        int lookupTable4D[64][4];

        //----------------------- Common variables    --------------------------------------
        int ii,jj,kk,ll;
        int gi0,gi1,gi2,gi3,gi4,gi5,gi6,gi7,gi8,gi9,gi10,gi11,gi12,gi13,gi14,gi15;

        //-----------------------  Simplex variables  --------------------------------------

        float n1, n2, n3, n4, n5;
        NzVector4f d1,d2,d3,d4,d5,unskewedCubeOrigin,unskewedDistToOrigin;
        NzVector4i off1, off2,off3,skewedCubeOrigin;


        float c1,c2,c3,c4,c5,c6;
        int c;

        float SkewCoeff2D;
        float UnskewCoeff2D;

        float SkewCoeff3D;
        float UnskewCoeff3D;

        float SkewCoeff4D;
        float UnskewCoeff4D;

        float sum;

        //-----------------------  Perlin Variables  -------------------------------------

        int x0,y0,z0,w0;
        float Li1,Li2,Li3,Li4,Li5,Li6,Li7,Li8,Li9,Li10,Li11,Li12,Li13,Li14;
        float s[4],t[4],u[4],v[4];
        float Cx, Cy, Cz, Cw;
        NzVector4f temp;
        float tmp;

        //----------------------  Complex Noise Variables --------------------------------


        bool first;
        float value;
        float remainder;
        float smax;
        float smin;

};

#endif // NOISEMACHINE_HPP

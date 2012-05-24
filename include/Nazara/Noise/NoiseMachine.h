// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NOISEMACHINE_HPP
#define NOISEMACHINE_HPP

#include <Nazara/Prerequesites.hpp>

class NzVector2f;
class NzVector3f;
class NzVector4f;

class NzNoiseMachine
{
    public:
        NzNoiseMachine(int seed = 0);
        ~NzNoiseMachine();

        void SetNewSeed(int seed);
        void ShufflePermutationTable();

        float Get1DPerlinNoiseValue(float x,                            float res);
        float Get2DPerlinNoiseValue(float x, float y,                   float res);
        float Get3DPerlinNoiseValue(float x, float y, float z,          float res);
        float Get4DPerlinNoiseValue(float x, float y, float z, float w, float res);

        float Get2DSimplexNoiseValue(float x, float y,                   float res);
        float Get3DSimplexNoiseValue(float x, float y, float z,          float res);
        float Get4DSimplexNoiseValue(float x, float y, float z, float w, float res);

        float Get2DCellNoiseValue(float x, float y,                   float res);
        float Get3DCellNoiseValue(float x, float y, float z,          float res);
        float Get4DCellNoiseValue(float x, float y, float z, float w, float res);

        void SetLacunarity(float lacunarity);
        void SetHurstParameter(float h);
        void SetOctavesNumber(float octaves);
        void RecomputeExponentArray();

        float Get1DFBMNoiseValue(float x,                   float res);
        float Get2DFBMNoiseValue(float x, float y,          float res);
        float Get3DFBMNoiseValue(float x, float y, float z, float res);

        float Get2DHybridMultiFractalNoiseValue(float x, float y,          float res);
        float Get3DHybridMultiFractalNoiseValue(float x, float y, float z, float res);

    protected:
    private:

        //Pour tronquer les nombres
        int fastfloor(float n);

        float pi;
        int perm[512];
        int PermutationTemp[256];
        float gradient2[8][2];
        int gradient3[16][3];
        int gradient4[32][4];
        int lookupTable4D[64][4];

        //multiplicative congruential generator
        int UcurrentSeed;
        int Ua,Uc,Um;
        int Uprevious;
        int Ulast;

        //-----------------------  Simplex variables  --------------------------------------

        float n1, n2, n3, n4, n5;
        NzVector4f A;
        NzVector4i Origin;
        NzVector4f d1,d2,d3,d4,d5;
        NzVector4i off1, off2,off3;
        NzVector4f IsoOriginDist;
        NzVector4f H[5];

        int ii,jj,kk,ll;
        int gi0,gi1,gi2,gi3,gi4,gi5,gi6,gi7,gi8,gi9,gi10,gi11,gi12,gi13,gi14,gi15;
        float lenght;
        float c1,c2,c3,c4,c5,c6;
        int c;

        float SkewCoeff2D;
        float UnskewCoeff2D;

        float SkewCoeff3D;
        float UnskewCoeff3D;

        float SkewCoeff4D;
        float UnskewCoeff4D;

        //-----------------------  Perlin Variables  -------------------------------------

        int x0,y0,z0,w0;
        float Li1,Li2,Li3,Li4,Li5,Li6,Li7,Li8,Li9,Li10,Li11,Li12,Li13,Li14;
        float s[4],t[4],u[4],v[4];
        float Cx, Cy, Cz, Cw;
        NzVector4f temp;
        float nx,ny,nz,nw;
        float tmp;

        //----------------------  Complex Noise Variables --------------------------------

        float m_lacunarity;
        float m_hurst;
        float m_octaves;
        bool m_parametersModified;
        float exponent_array[30];
        bool first;
        float value;
        float remainder;

        float smax;
        float smin;

};

#endif // NOISEMACHINE_H

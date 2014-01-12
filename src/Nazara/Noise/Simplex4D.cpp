// Copyright (C) 2014 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Simplex4D.hpp>
#include <Nazara/Noise/Debug.hpp>

NzSimplex4D::NzSimplex4D()
{
    SkewCoeff4D = (std::sqrt(5.f) - 1.f)/4.f;
    UnskewCoeff4D = (5.f - std::sqrt(5.f))/20.f;

    int lookupTemp4D[][4] =
    {
        {0,1,2,3},{0,1,3,2},{0,0,0,0},{0,2,3,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,2,3,0},
        {0,2,1,3},{0,0,0,0},{0,3,1,2},{0,3,2,1},{0,0,0,0},{0,0,0,0},{0,0,0,0},{1,3,2,0},
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
        {1,2,0,3},{0,0,0,0},{1,3,0,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,3,0,1},{2,3,1,0},
        {1,0,2,3},{1,0,3,2},{0,0,0,0},{0,0,0,0},{0,0,0,0},{2,0,3,1},{0,0,0,0},{2,1,3,0},
        {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
        {2,0,1,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,0,1,2},{3,0,2,1},{0,0,0,0},{3,1,2,0},
        {2,1,0,3},{0,0,0,0},{0,0,0,0},{0,0,0,0},{3,1,0,2},{0,0,0,0},{3,2,0,1},{3,2,1,0}
    };

    for(int i(0) ; i < 64 ; ++i)
        for(int j(0) ; j < 4 ; ++j)
            lookupTable4D[i][j] = lookupTemp4D[i][j];

    float grad4Temp[][4] =
    {
        {0.f,1.f,1.f,1.f}, {0.f,1.f,1.f,-1.f}, {0.f,1.f,-1.f,1.f}, {0.f,1.f,-1.f,-1.f},
        {0.f,-1.f,1.f,1.f},{0.f,-1.f,1.f,-1.f},{0.f,-1.f,-1.f,1.f},{0.f,-1.f,-1.f,-1.f},
        {1.f,0.f,1.f,1.f}, {1.f,0.f,1.f,-1.f}, {1.f,0.f,-1.f,1.f}, {1.f,0.f,-1.f,-1.f},
        {-1.f,0.f,1.f,1.f},{-1.f,0.f,1.f,-1.f},{-1.f,0.f,-1.f,1.f},{-1.f,0.f,-1.f,-1.f},
        {1.f,1.f,0.f,1.f}, {1.f,1.f,0.f,-1.f}, {1.f,-1.f,0.f,1.f}, {1.f,-1.f,0.f,-1.f},
        {-1.f,1.f,0.f,1.f},{-1.f,1.f,0.f,-1.f},{-1.f,-1.f,0.f,1.f},{-1.f,-1.f,0.f,-1.f},
        {1.f,1.f,1.f,0.f}, {1.f,1.f,-1.f,0.f}, {1.f,-1.f,1.f,0.f}, {1.f,-1.f,-1.f,0.f},
        {-1.f,1.f,1.f,0.f},{-1.f,1.f,-1.f,0.f},{-1.f,-1.f,1.f,0.f},{-1.f,-1.f,-1.f,0.f}
    };

    for(int i(0) ; i < 32 ; ++i)
        for(int j(0) ; j < 4 ; ++j)
            gradient4[i][j] = grad4Temp[i][j];
}

NzSimplex4D::NzSimplex4D(unsigned int seed) : NzSimplex4D()
{
    this->SetNewSeed(seed);
    this->ShufflePermutationTable();
}

float NzSimplex4D::GetValue(float x, float y, float z, float w, float resolution)
{
    x *= resolution;
    y *= resolution;
    z *= resolution;
    w *= resolution;

    sum = (x + y + z + w) * SkewCoeff4D;
    skewedCubeOrigin.x = fastfloor(x + sum);
    skewedCubeOrigin.y = fastfloor(y + sum);
    skewedCubeOrigin.z = fastfloor(z + sum);
    skewedCubeOrigin.w = fastfloor(w + sum);

    sum = (skewedCubeOrigin.x + skewedCubeOrigin.y + skewedCubeOrigin.z + skewedCubeOrigin.w) * UnskewCoeff4D;
    unskewedCubeOrigin.x = skewedCubeOrigin.x - sum;
    unskewedCubeOrigin.y = skewedCubeOrigin.y - sum;
    unskewedCubeOrigin.z = skewedCubeOrigin.z - sum;
    unskewedCubeOrigin.w = skewedCubeOrigin.w - sum;

    unskewedDistToOrigin.x = x - unskewedCubeOrigin.x;
    unskewedDistToOrigin.y = y - unskewedCubeOrigin.y;
    unskewedDistToOrigin.z = z - unskewedCubeOrigin.z;
    unskewedDistToOrigin.w = w - unskewedCubeOrigin.w;

    c1 = (unskewedDistToOrigin.x > unskewedDistToOrigin.y) ? 32 : 0;
    c2 = (unskewedDistToOrigin.x > unskewedDistToOrigin.z) ? 16 : 0;
    c3 = (unskewedDistToOrigin.y > unskewedDistToOrigin.z) ? 8  : 0;
    c4 = (unskewedDistToOrigin.x > unskewedDistToOrigin.w) ? 4  : 0;
    c5 = (unskewedDistToOrigin.y > unskewedDistToOrigin.w) ? 2  : 0;
    c6 = (unskewedDistToOrigin.z > unskewedDistToOrigin.w) ? 1  : 0;
    c = c1 + c2 + c3 + c4 + c5 + c6;

    off1.x = lookupTable4D[c][0] >= 3 ? 1 : 0;
    off1.y = lookupTable4D[c][1] >= 3 ? 1 : 0;
    off1.z = lookupTable4D[c][2] >= 3 ? 1 : 0;
    off1.w = lookupTable4D[c][3] >= 3 ? 1 : 0;

    off2.x = lookupTable4D[c][0] >= 2 ? 1 : 0;
    off2.y = lookupTable4D[c][1] >= 2 ? 1 : 0;
    off2.z = lookupTable4D[c][2] >= 2 ? 1 : 0;
    off2.w = lookupTable4D[c][3] >= 2 ? 1 : 0;

    off3.x = lookupTable4D[c][0] >= 1 ? 1 : 0;
    off3.y = lookupTable4D[c][1] >= 1 ? 1 : 0;
    off3.z = lookupTable4D[c][2] >= 1 ? 1 : 0;
    off3.w = lookupTable4D[c][3] >= 1 ? 1 : 0;

    d1 = unskewedDistToOrigin;

    d2.x = d1.x - off1.x + UnskewCoeff4D;
    d2.y = d1.y - off1.y + UnskewCoeff4D;
    d2.z = d1.z - off1.z + UnskewCoeff4D;
    d2.w = d1.w - off1.w + UnskewCoeff4D;

    d3.x = d1.x - off2.x + 2.f*UnskewCoeff4D;
    d3.y = d1.y - off2.y + 2.f*UnskewCoeff4D;
    d3.z = d1.z - off2.z + 2.f*UnskewCoeff4D;
    d3.w = d1.w - off2.w + 2.f*UnskewCoeff4D;

    d4.x = d1.x - off3.x + 3.f*UnskewCoeff4D;
    d4.y = d1.y - off3.y + 3.f*UnskewCoeff4D;
    d4.z = d1.z - off3.z + 3.f*UnskewCoeff4D;
    d4.w = d1.w - off3.w + 3.f*UnskewCoeff4D;

    d5.x = d1.x - 1.f + 4*UnskewCoeff4D;
    d5.y = d1.y - 1.f + 4*UnskewCoeff4D;
    d5.z = d1.z - 1.f + 4*UnskewCoeff4D;
    d5.w = d1.w - 1.f + 4*UnskewCoeff4D;

    ii = skewedCubeOrigin.x & 255;
    jj = skewedCubeOrigin.y & 255;
    kk = skewedCubeOrigin.z & 255;
    ll = skewedCubeOrigin.w & 255;

    gi0 = perm[ii +          perm[jj +          perm[kk +          perm[ll]]]] & 31;
    gi1 = perm[ii + off1.x + perm[jj + off1.y + perm[kk + off1.z + perm[ll + off1.w]]]] & 31;
    gi2 = perm[ii + off2.x + perm[jj + off2.y + perm[kk + off2.z + perm[ll + off2.w]]]] & 31;
    gi3 = perm[ii + off3.x + perm[jj + off3.y + perm[kk + off3.z + perm[ll + off3.w]]]] & 31;
    gi4 = perm[ii + 1 +      perm[jj + 1 +      perm[kk + 1 +      perm[ll + 1]]]] % 32;

    c1 = 0.6f - d1.x*d1.x - d1.y*d1.y - d1.z*d1.z - d1.w*d1.w;
    c2 = 0.6f - d2.x*d2.x - d2.y*d2.y - d2.z*d2.z - d2.w*d2.w;
    c3 = 0.6f - d3.x*d3.x - d3.y*d3.y - d3.z*d3.z - d3.w*d3.w;
    c4 = 0.6f - d4.x*d4.x - d4.y*d4.y - d4.z*d4.z - d4.w*d4.w;
    c5 = 0.6f - d5.x*d5.x - d5.y*d5.y - d5.z*d5.z - d5.w*d5.w;

    if(c1 < 0)
        n1 = 0;
    else
        n1 = c1*c1*c1*c1*(gradient4[gi0][0]*d1.x + gradient4[gi0][1]*d1.y + gradient4[gi0][2]*d1.z + gradient4[gi0][3]*d1.w);

    if(c2 < 0)
        n2 = 0;
    else
        n2 = c2*c2*c2*c2*(gradient4[gi1][0]*d2.x + gradient4[gi1][1]*d2.y + gradient4[gi1][2]*d2.z + gradient4[gi1][3]*d2.w);

    if(c3 < 0)
        n3 = 0;
    else
        n3 = c3*c3*c3*c3*(gradient4[gi2][0]*d3.x + gradient4[gi2][1]*d3.y + gradient4[gi2][2]*d3.z + gradient4[gi2][3]*d3.w);

    if(c4 < 0)
        n4 = 0;
    else
        n4 = c4*c4*c4*c4*(gradient4[gi3][0]*d4.x + gradient4[gi3][1]*d4.y + gradient4[gi3][2]*d4.z + gradient4[gi3][3]*d4.w);

    if(c5 < 0)
        n5 = 0;
    else
        n5 = c5*c5*c5*c5*(gradient4[gi4][0]*d5.x + gradient4[gi4][1]*d5.y + gradient4[gi4][2]*d5.z + gradient4[gi4][3]*d5.w);

    return (n1+n2+n3+n4+n5)*27.f;
}

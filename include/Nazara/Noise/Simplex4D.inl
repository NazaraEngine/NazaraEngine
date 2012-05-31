// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

//#include <Nazara/Noise/Error.hpp>
//#include <Nazara/Noise/Config.hpp>
//#include <Nazara/Noise/Debug.hpp>

template <typename T>
NzSimplex4D<T>::NzSimplex4D()
{
    SkewCoeff4D = (sqrt(5) - 1)/4;
    UnskewCoeff4D = (5 - sqrt(5))/20;

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

    int grad4Temp[][4] =
    {
        {0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1},
        {0,-1,1,1},{0,-1,1,-1},{0,-1,-1,1},{0,-1,-1,-1},
        {1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
        {-1,0,1,1},{-1,0,1,-1},{-1,0,-1,1},{-1,0,-1,-1},
        {1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
        {-1,1,0,1},{-1,1,0,-1},{-1,-1,0,1},{-1,-1,0,-1},
        {1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
        {-1,1,1,0},{-1,1,-1,0},{-1,-1,1,0},{-1,-1,-1,0}
    };

    for(int i(0) ; i < 32 ; ++i)
        for(int j(0) ; j < 4 ; ++j)
            gradient4[i][j] = grad4Temp[i][j];
}

template <typename T>
T NzSimplex4D<T>::GetValue(T x, T y, T z, T w, T res)
{
    x /= res;
    y /= res;
    z /= res;
    w /= res;

    Origin.x = fastfloor(x + (x + y + z + w) * SkewCoeff4D);
    Origin.y = fastfloor(y + (x + y + z + w) * SkewCoeff4D);
    Origin.z = fastfloor(z + (x + y + z + w) * SkewCoeff4D);
    Origin.w = fastfloor(w + (x + y + z + w) * SkewCoeff4D);

    A.x = Origin.x - (Origin.x + Origin.y + Origin.z + Origin.w) * UnskewCoeff4D;
    A.y = Origin.y - (Origin.x + Origin.y + Origin.z + Origin.w) * UnskewCoeff4D;
    A.z = Origin.z - (Origin.x + Origin.y + Origin.z + Origin.w) * UnskewCoeff4D;
    A.w = Origin.w - (Origin.x + Origin.y + Origin.z + Origin.w) * UnskewCoeff4D;

    IsoOriginDist.x = x - A.x;
    IsoOriginDist.y = y - A.y;
    IsoOriginDist.z = z - A.z;
    IsoOriginDist.w = w - A.w;

    c1 = (IsoOriginDist.x > IsoOriginDist.y) ? 32 : 0;
    c2 = (IsoOriginDist.x > IsoOriginDist.z) ? 16 : 0;
    c3 = (IsoOriginDist.y > IsoOriginDist.z) ? 8  : 0;
    c4 = (IsoOriginDist.x > IsoOriginDist.w) ? 4  : 0;
    c5 = (IsoOriginDist.y > IsoOriginDist.w) ? 2  : 0;
    c6 = (IsoOriginDist.z > IsoOriginDist.w) ? 1  : 0;
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

    d1.x = A.x - x;
    d1.y = A.y - y;
    d1.z = A.z - z;
    d1.w = A.w - w;

    d2.x = d1.x + off1.x - UnskewCoeff4D;
    d2.y = d1.y + off1.y - UnskewCoeff4D;
    d2.z = d1.z + off1.z - UnskewCoeff4D;
    d2.w = d1.w + off1.w - UnskewCoeff4D;

    d3.x = d1.x + off2.x - 2*UnskewCoeff4D;
    d3.y = d1.y + off2.y - 2*UnskewCoeff4D;
    d3.z = d1.z + off2.z - 2*UnskewCoeff4D;
    d3.w = d1.w + off2.w - 2*UnskewCoeff4D;

    d4.x = d1.x + off3.x - 3*UnskewCoeff4D;
    d4.y = d1.y + off3.y - 3*UnskewCoeff4D;
    d4.z = d1.z + off3.z - 3*UnskewCoeff4D;
    d4.w = d1.w + off3.w - 3*UnskewCoeff4D;

    d5.x = d1.x + 1.0 - 4*UnskewCoeff4D;
    d5.y = d1.y + 1.0 - 4*UnskewCoeff4D;
    d5.z = d1.z + 1.0 - 4*UnskewCoeff4D;
    d5.w = d1.w + 1.0 - 4*UnskewCoeff4D;

    ii = Origin.x & 255;
    jj = Origin.y & 255;
    kk = Origin.z & 255;
    ll = Origin.w & 255;

    gi0 = perm[ii + perm[jj + perm[kk + perm[ll]]]] % 32;
    gi1 = perm[ii + off1.x + perm[jj + off1.y + perm[kk + off1.z + perm[ll + off1.w]]]] % 32;
    gi2 = perm[ii + off2.x + perm[jj + off2.y + perm[kk + off2.z + perm[ll + off2.w]]]] % 32;
    gi3 = perm[ii + off3.x + perm[jj + off3.y + perm[kk + off3.z + perm[ll + off3.w]]]] % 32;
    gi4 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll + 1]]]] % 32;

    n1 = gradient4[gi0][0]*d1.x + gradient4[gi0][1]*d1.y + gradient4[gi0][2]*d1.z + gradient4[gi0][3]*d1.w;
    n2 = gradient4[gi1][0]*d2.x + gradient4[gi1][1]*d2.y + gradient4[gi1][2]*d2.z + gradient4[gi1][3]*d2.w;
    n3 = gradient4[gi2][0]*d3.x + gradient4[gi2][1]*d3.y + gradient4[gi2][2]*d3.z + gradient4[gi2][3]*d3.w;
    n4 = gradient4[gi3][0]*d4.x + gradient4[gi3][1]*d4.y + gradient4[gi3][2]*d4.z + gradient4[gi3][3]*d4.w;
    n5 = gradient4[gi4][0]*d5.x + gradient4[gi4][1]*d5.y + gradient4[gi4][2]*d5.z + gradient4[gi4][3]*d5.w;

    c1 = 0.6 - d1.x*d1.x - d1.y*d1.y - d1.z*d1.z - d1.w*d1.w;
    c2 = 0.6 - d2.x*d2.x - d2.y*d2.y - d2.z*d2.z - d2.w*d2.w;
    c3 = 0.6 - d3.x*d3.x - d3.y*d3.y - d3.z*d3.z - d3.w*d3.w;
    c4 = 0.6 - d4.x*d4.x - d4.y*d4.y - d4.z*d4.z - d4.w*d4.w;
    c5 = 0.6 - d5.x*d5.x - d5.y*d5.y - d5.z*d5.z - d5.w*d5.w;

    if(c1 < 0)
        c1 = 0;
    if(c2 < 0)
        c2 = 0;
    if(c3 < 0)
        c3 = 0;
    if(c4 < 0)
        c4 = 0;
    if(c5 < 0)
        c5 = 0;

    n1 = c1*c1*c1*n1;
    n2 = c2*c2*c2*n2;
    n3 = c3*c3*c3*n3;
    n4 = c4*c4*c4*n4;
    n5 = c5*c5*c5*n5;

    return (n1+n2+n3+n4+n5)*17.6995;
}


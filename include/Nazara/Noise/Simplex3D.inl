// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

//#include <Nazara/Noise/Error.hpp>
//#include <Nazara/Noise/Config.hpp>
//#include <Nazara/Noise/Debug.hpp>

template <typename T>
NzSimplex3D<T>::NzSimplex3D()
{
    SkewCoeff3D = 1/3;
    UnskewCoeff3D = 1/6;

    int grad3Temp[][3] = {
        {1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
        {1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
        {0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1},
        {1,1,0},{-1,1,0},{0,-1,1},{0,-1,-1}
    };

    for(int i(0) ; i < 16 ; ++i)
        for(int j(0) ; j < 3 ; ++j)
            gradient3[i][j] = grad3Temp[i][j];
}

template <typename T>
T NzSimplex3D<T>::GetValue(T x, T y, T z, T res)
{
    x /= res;
    y /= res;
    z /= res;

    Origin.x = fastfloor(x + (x + y + z) * SkewCoeff3D);
    Origin.y = fastfloor(y + (x + y + z) * SkewCoeff3D);
    Origin.z = fastfloor(z + (x + y + z) * SkewCoeff3D);

    A.x = Origin.x - (Origin.x + Origin.y + Origin.z) * UnskewCoeff3D;
    A.y = Origin.y - (Origin.x + Origin.y + Origin.z) * UnskewCoeff3D;
    A.z = Origin.z - (Origin.x + Origin.y + Origin.z) * UnskewCoeff3D;

    IsoOriginDist.x = x - A.x;
    IsoOriginDist.y = y - A.y;
    IsoOriginDist.z = z - A.z;

    if(IsoOriginDist.x >= IsoOriginDist.y)
    {
        if(IsoOriginDist.y >= IsoOriginDist.z)
        {
            off1.x = 1;
            off1.y = 0;
            off1.z = 0;
            off2.x = 1;
            off2.y = 1;
            off2.z = 0;
        }
        else if(IsoOriginDist.x >= IsoOriginDist.z)
        {
            off1.x = 1;
            off1.y = 0;
            off1.z = 0;
            off2.x = 1;
            off2.y = 0;
            off2.z = 1;
        }
        else
        {
            off1.x = 0;
            off1.y = 0;
            off1.z = 1;
            off2.x = 1;
            off2.y = 0;
            off2.z = 1;
        }
    }
    else
    {
        if(IsoOriginDist.y < IsoOriginDist.z)
        {
            off1.x = 0;
            off1.y = 0;
            off1.z = 1;
            off2.x = 0;
            off2.y = 1;
            off2.z = 1;
        }
        else if(IsoOriginDist.x < IsoOriginDist.z)
        {
            off1.x = 0;
            off1.y = 1;
            off1.z = 0;
            off2.x = 0;
            off2.y = 1;
            off2.z = 1;
        }
        else
        {
            off1.x = 0;
            off1.y = 1;
            off1.z = 0;
            off2.x = 1;
            off2.y = 1;
            off2.z = 0;
        }
    }

    d1.x = A.x - x;
    d1.y = A.y - y;
    d1.z = A.z - z;

    d2.x = d1.x + off1.x - UnskewCoeff3D;
    d2.y = d1.y + off1.y - UnskewCoeff3D;
    d2.z = d1.z + off1.z - UnskewCoeff3D;

    d3.x = d1.x + off2.x - 2*UnskewCoeff3D;
    d3.y = d1.y + off2.y - 2*UnskewCoeff3D;
    d3.z = d1.z + off2.z - 2*UnskewCoeff3D;

    d4.x = d1.x + 1.0 - 3*UnskewCoeff3D;
    d4.y = d1.y + 1.0 - 3*UnskewCoeff3D;
    d4.z = d1.z + 1.0 - 3*UnskewCoeff3D;

    ii = Origin.x & 255;
    jj = Origin.y & 255;
    kk = Origin.z & 255;

    gi0 = perm[ii + perm[jj + perm[kk]]] % 12;
    gi1 = perm[ii + off1.x + perm[jj + off1.y + perm[kk + off1.z]]] % 12;
    gi2 = perm[ii + off2.x + perm[jj + off2.y + perm[kk + off2.z]]] % 12;
    gi3 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] % 12;

    n1 = gradient3[gi0][0] * d1.x + gradient3[gi0][1] * d1.y + gradient3[gi0][2] * d1.z;
    n2 = gradient3[gi1][0] * d2.x + gradient3[gi1][1] * d2.y + gradient3[gi1][2] * d2.z;
    n3 = gradient3[gi2][0] * d3.x + gradient3[gi2][1] * d3.y + gradient3[gi2][2] * d3.z;
    n4 = gradient3[gi3][0] * d4.x + gradient3[gi3][1] * d4.y + gradient3[gi3][2] * d4.z;

    c1 = 0.6 - d1.x * d1.x - d1.y * d1.y - d1.z * d1.z;
    c2 = 0.6 - d2.x * d2.x - d2.y * d2.y - d2.z * d2.z;
    c3 = 0.6 - d3.x * d3.x - d3.y * d3.y - d3.z * d3.z;
    c4 = 0.6 - d4.x * d4.x - d4.y * d4.y - d4.z * d4.z;

    if(c1 < 0)
        c1 = 0;
    if(c2 < 0)
        c2 = 0;
    if(c3 < 0)
        c3 = 0;
    if(c4 < 0)
        c4 = 0;

    n1 = c1*c1*c1*n1;
    n2 = c2*c2*c2*n2;
    n3 = c3*c3*c3*n3;
    n4 = c4*c4*c4*n4;

    return (n1+n2+n3+n4)*17.6995;
}


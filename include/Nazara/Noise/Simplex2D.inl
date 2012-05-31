// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

//#include <Nazara/Noise/Error.hpp>
//#include <Nazara/Noise/Config.hpp>
//#include <Nazara/Noise/Debug.hpp>

template <typename T>
NzSimplex2D<T>::NzSimplex2D()
{

    T unit = 1.0/sqrt(2);
    T grad2Temp[][2] = {{unit,unit},{-unit,unit},{unit,-unit},{-unit,-unit},
                            {1,0},{-1,0},{0,1},{0,-1}};

    for(int i(0) ; i < 8 ; ++i)
        for(int j(0) ; j < 2 ; ++j)
            gradient2[i][j] = grad2Temp[i][j];

    SkewCoeff2D = 0.5*(sqrt(3.0) - 1.0);
    UnskewCoeff2D  = (3.0-sqrt(3.0))/6;
}

template <typename T>
T NzSimplex2D<T>::GetValue(T x, T y, T res)
{
    x /= res;
    y /= res;

    Origin.x = fastfloor(x + (x + y) * SkewCoeff2D);
    Origin.y = fastfloor(y + (x + y) * SkewCoeff2D);

    A.x = Origin.x - (Origin.x + Origin.y) * UnskewCoeff2D;
    A.y = Origin.y - (Origin.x + Origin.y) * UnskewCoeff2D;

    IsoOriginDist.x = x - A.x;
    IsoOriginDist.y = y - A.y;

    if(IsoOriginDist.x > IsoOriginDist.y)
    {
        off1.x = 1;
        off1.y = 0;
    }
    else
    {
        off1.x = 0;
        off1.y = 1;
    }

    d1.x = A.x - x;
    d1.y = A.y - y;

    d2.x = d1.x + off1.x - UnskewCoeff2D;
    d2.y = d1.y + off1.y - UnskewCoeff2D;

    d3.x = d1.x + 1.0 - 2 * UnskewCoeff2D;
    d3.y = d1.y + 1.0 - 2 * UnskewCoeff2D;

    ii = Origin.x & 255;
    jj = Origin.y & 255;

    gi0 = perm[ii + perm[jj]] % 8;
    gi1 = perm[ii + off1.x + perm[jj + off1.y]] % 8;
    gi2 = perm[ii + 1 + perm[jj + 1]] % 8;

    n1 = gradient2[gi0][0] * d1.x + gradient2[gi0][1] * d1.y;
    n2 = gradient2[gi1][0] * d2.x + gradient2[gi1][1] * d2.y;
    n3 = gradient2[gi2][0] * d3.x + gradient2[gi2][1] * d3.y;

    c1 = 0.5 - d1.x * d1.x - d1.y * d1.y;
    c2 = 0.5 - d2.x * d2.x - d2.y * d2.y;
    c3 = 0.5 - d3.x * d3.x - d3.y * d3.y;

    if(c1 < 0)
        c1 = 0;
    if(c2 < 0)
        c2 = 0;
    if(c3 < 0)
        c3 = 0;

    n1 = c1*c1*c1*n1;
    n2 = c2*c2*c2*n2;
    n3 = c3*c3*c3*n3;

    return (n1+n2+n3)*23.2;
}


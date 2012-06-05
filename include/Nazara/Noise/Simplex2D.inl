// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

//#include <Nazara/Noise/Error.hpp>
//#include <Nazara/Noise/Config.hpp>
//#include <Nazara/Noise/Debug.hpp>

template <typename T>
NzSimplex2D<T>::NzSimplex2D()
{
    T grad2Temp[][2] = {{1,1},{-1,1},{1,-1},{-1,-1},
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

    skewedCubeOrigin.x = fastfloor(x + (x + y) * SkewCoeff2D);
    skewedCubeOrigin.y = fastfloor(y + (x + y) * SkewCoeff2D);

    unskewedCubeOrigin.x = skewedCubeOrigin.x - (skewedCubeOrigin.x + skewedCubeOrigin.y) * UnskewCoeff2D;
    unskewedCubeOrigin.y = skewedCubeOrigin.y - (skewedCubeOrigin.x + skewedCubeOrigin.y) * UnskewCoeff2D;

    unskewedDistToOrigin.x = x - unskewedCubeOrigin.x;
    unskewedDistToOrigin.y = y - unskewedCubeOrigin.y;

    if(unskewedDistToOrigin.x > unskewedDistToOrigin.y)
    {
        off1.x = 1;
        off1.y = 0;
    }
    else
    {
        off1.x = 0;
        off1.y = 1;
    }


    d1.x = unskewedCubeOrigin.x - x;
    d1.y = unskewedCubeOrigin.y - y;

    d2.x = d1.x + off1.x - UnskewCoeff2D;
    d2.y = d1.y + off1.y - UnskewCoeff2D;

    d3.x = d1.x + 1.0 - 2 * UnskewCoeff2D;
    d3.y = d1.y + 1.0 - 2 * UnskewCoeff2D;

    ii = skewedCubeOrigin.x & 255;
    jj = skewedCubeOrigin.y & 255;

    gi0 = perm[ii + perm[jj]] & 7;
    gi1 = perm[ii + off1.x + perm[jj + off1.y]] & 7;
    gi2 = perm[ii + 1 + perm[jj + 1]] & 7;

    c1 = 0.5 - d1.x * d1.x - d1.y * d1.y;
    c2 = 0.5 - d2.x * d2.x - d2.y * d2.y;
    c3 = 0.5 - d3.x * d3.x - d3.y * d3.y;

    if(c1 < 0)
        n1 = 0;
    else
        n1 = c1*c1*c1*c1*(gradient2[gi0][0] * d1.x + gradient2[gi0][1] * d1.y);

    if(c2 < 0)
        n2 = 0;
    else
        n2 = c2*c2*c2*c2*(gradient2[gi1][0] * d2.x + gradient2[gi1][1] * d2.y);

    if(c3 < 0)
        n3 = 0;
    else
        n3 = c3*c3*c3*c3*(gradient2[gi2][0] * d3.x + gradient2[gi2][1] * d3.y);

    return (n1+n2+n3)*70;
}


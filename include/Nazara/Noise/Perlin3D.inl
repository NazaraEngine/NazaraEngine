// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

template <typename T>
NzPerlin3D<T>::NzPerlin3D()
{
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
T NzPerlin3D<T>::GetValue(T x, T y, T z, T res)
{
    x /= res;
    y /= res;
    z /= res;

    x0 = fastfloor(x);
    y0 = fastfloor(y);
    z0 = fastfloor(z);

    ii = x0 & 255;
    jj = y0 & 255;
    kk = z0 & 255;

    gi0 = perm[ii + perm[jj + perm[kk]]] & 15;
    gi1 = perm[ii + 1 + perm[jj + perm[kk]]] & 15;
    gi2 = perm[ii + perm[jj + 1 + perm[kk]]] & 15;
    gi3 = perm[ii + 1 + perm[jj + 1 + perm[kk]]] & 15;

    gi4 = perm[ii + perm[jj + perm[kk + 1]]] & 15;
    gi5 = perm[ii + 1 + perm[jj + perm[kk + 1]]] & 15;
    gi6 = perm[ii + perm[jj + 1 + perm[kk + 1]]] & 15;
    gi7 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] & 15;

    temp.x = x-x0;
    temp.y = y-y0;
    temp.z = z-z0;

    Cx = temp.x * temp.x * temp.x * (temp.x * (temp.x * 6 - 15) + 10);
    Cy = temp.y * temp.y * temp.y * (temp.y * (temp.y * 6 - 15) + 10);
    Cz = temp.z * temp.z * temp.z * (temp.z * (temp.z * 6 - 15) + 10);

    s[0] = gradient3[gi0][0]*temp.x + gradient3[gi0][1]*temp.y + gradient3[gi0][2]*temp.z;

    temp.x = x-(x0+1);
    t[0] = gradient3[gi1][0]*temp.x + gradient3[gi1][1]*temp.y + gradient3[gi1][2]*temp.z;

    temp.y = y-(y0+1);
    v[0] = gradient3[gi3][0]*temp.x + gradient3[gi3][1]*temp.y + gradient3[gi3][2]*temp.z;

    temp.x = x-x0;
    u[0] = gradient3[gi2][0]*temp.x + gradient3[gi2][1]*temp.y + gradient3[gi2][2]*temp.z;

    temp.y = y-y0;
    temp.z = z-(z0+1);
    s[1] = gradient3[gi4][0]*temp.x + gradient3[gi4][1]*temp.y + gradient3[gi4][2]*temp.z;

    temp.x = x-(x0+1);
    t[1] = gradient3[gi5][0]*temp.x + gradient3[gi5][1]*temp.y + gradient3[gi5][2]*temp.z;

    temp.y = y-(y0+1);
    v[1] = gradient3[gi7][0]*temp.x + gradient3[gi7][1]*temp.y + gradient3[gi7][2]*temp.z;

    temp.x = x-x0;
    u[1] = gradient3[gi6][0]*temp.x + gradient3[gi6][1]*temp.y + gradient3[gi6][2]*temp.z;

    Li1 = s[0] + Cx*(t[0]-s[0]);
    Li2 = u[0] + Cx*(v[0]-u[0]);
    Li3 = s[1] + Cx*(t[1]-s[1]);
    Li4 = u[1] + Cx*(v[1]-u[1]);

    Li5 = Li1 + Cy*(Li2-Li1);
    Li6 = Li3 + Cy*(Li4-Li3);

    return Li5 + Cz*(Li6-Li5);
}

#include <Nazara/Core/DebugOff.hpp>

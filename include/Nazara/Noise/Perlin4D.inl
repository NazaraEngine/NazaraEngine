// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

template <typename T>
NzPerlin4D<T>::NzPerlin4D()
{
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
T NzPerlin4D<T>::GetValue(T x, T y, T z, T w, T resolution)
{
    x *= resolution;
    y *= resolution;
    z *= resolution;
    w *= resolution;

    x0 = fastfloor(x);
    y0 = fastfloor(y);
    z0 = fastfloor(z);
    w0 = fastfloor(w);

    ii = x0 & 255;
    jj = y0 & 255;
    kk = z0 & 255;
    ll = w0 & 255;

    gi0 =  perm[ii     + perm[jj     + perm[kk     + perm[ll    ]]]] & 31;
    gi1 =  perm[ii + 1 + perm[jj     + perm[kk     + perm[ll    ]]]] & 31;
    gi2 =  perm[ii     + perm[jj + 1 + perm[kk     + perm[ll    ]]]] & 31;
    gi3 =  perm[ii + 1 + perm[jj + 1 + perm[kk     + perm[ll    ]]]] & 31;

    gi4 =  perm[ii     + perm[jj +   + perm[kk + 1 + perm[ll    ]]]] & 31;
    gi5 =  perm[ii + 1 + perm[jj +   + perm[kk + 1 + perm[ll    ]]]] & 31;
    gi6 =  perm[ii     + perm[jj + 1 + perm[kk + 1 + perm[ll    ]]]] & 31;
    gi7 =  perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll    ]]]] & 31;

    gi8 =  perm[ii     + perm[jj     + perm[kk     + perm[ll + 1]]]] & 31;
    gi9 =  perm[ii + 1 + perm[jj     + perm[kk     + perm[ll + 1]]]] & 31;
    gi10 = perm[ii     + perm[jj + 1 + perm[kk     + perm[ll + 1]]]] & 31;
    gi11 = perm[ii + 1 + perm[jj + 1 + perm[kk     + perm[ll + 1]]]] & 31;

    gi12 = perm[ii     + perm[jj     + perm[kk + 1 + perm[ll + 1]]]] & 31;
    gi13 = perm[ii + 1 + perm[jj     + perm[kk + 1 + perm[ll + 1]]]] & 31;
    gi14 = perm[ii     + perm[jj + 1 + perm[kk + 1 + perm[ll + 1]]]] & 31;
    gi15 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll + 1]]]] & 31;

    temp.x = x-x0;
    temp.y = y-y0;
    temp.z = z-z0;
    temp.w = w-w0;

    Cx = temp.x * temp.x * temp.x * (temp.x * (temp.x * 6 - 15) + 10);
    Cy = temp.y * temp.y * temp.y * (temp.y * (temp.y * 6 - 15) + 10);
    Cz = temp.z * temp.z * temp.z * (temp.z * (temp.z * 6 - 15) + 10);
    Cw = temp.w * temp.w * temp.w * (temp.w * (temp.w * 6 - 15) + 10);

    s[0] = gradient4[gi0][0]*temp.x + gradient4[gi0][1]*temp.y + gradient4[gi0][2]*temp.z + gradient4[gi0][3]*temp.w;

    temp.x = x-(x0+1);
    t[0] = gradient4[gi1][0]*temp.x + gradient4[gi1][1]*temp.y + gradient4[gi1][2]*temp.z + gradient4[gi1][3]*temp.w;

    temp.y = y-(y0+1);
    v[0] = gradient4[gi3][0]*temp.x + gradient4[gi3][1]*temp.y + gradient4[gi3][2]*temp.z + gradient4[gi3][3]*temp.w;

    temp.x = x-x0;
    u[0] = gradient4[gi2][0]*temp.x + gradient4[gi2][1]*temp.y + gradient4[gi2][2]*temp.z + gradient4[gi2][3]*temp.w;

    temp.y = y-y0;
    temp.z = z-(z0+1);
    s[1] = gradient4[gi4][0]*temp.x + gradient4[gi4][1]*temp.y + gradient4[gi4][2]*temp.z + gradient4[gi4][3]*temp.w;

    temp.x = x-(x0+1);
    t[1] = gradient4[gi5][0]*temp.x + gradient4[gi5][1]*temp.y + gradient4[gi5][2]*temp.z + gradient4[gi5][3]*temp.w;

    temp.y = y-(y0+1);
    v[1] = gradient4[gi7][0]*temp.x + gradient4[gi7][1]*temp.y + gradient4[gi7][2]*temp.z + gradient4[gi7][3]*temp.w;

    temp.x = x-x0;
    u[1] = gradient4[gi6][0]*temp.x + gradient4[gi6][1]*temp.y + gradient4[gi6][2]*temp.z + gradient4[gi6][3]*temp.w;


    temp.y = y-y0;
    temp.z = z-z0;
    temp.w = w-(w0+1);
    s[2] = gradient4[gi8][0]*temp.x + gradient4[gi8][1]*temp.y + gradient4[gi8][2]*temp.z + gradient4[gi8][3]*temp.w;

    temp.x = x-(x0+1);
    t[2] = gradient4[gi9][0]*temp.x + gradient4[gi9][1]*temp.y + gradient4[gi9][2]*temp.z + gradient4[gi9][3]*temp.w;

    temp.y = y-(y0+1);
    v[2] = gradient4[gi11][0]*temp.x + gradient4[gi11][1]*temp.y + gradient4[gi11][2]*temp.z + gradient4[gi11][3]*temp.w;

    temp.x = x-x0;
    u[2] = gradient4[gi10][0]*temp.x + gradient4[gi10][1]*temp.y + gradient4[gi10][2]*temp.z + gradient4[gi10][3]*temp.w;


    temp.y = y-y0;
    temp.z = z-(z0+1);
    s[3] = gradient4[gi12][0]*temp.x + gradient4[gi12][1]*temp.y + gradient4[gi12][2]*temp.z + gradient4[gi12][3]*temp.w;

    temp.x = x-(x0+1);
    t[3] = gradient4[gi13][0]*temp.x + gradient4[gi13][1]*temp.y + gradient4[gi13][2]*temp.z + gradient4[gi13][3]*temp.w;

    temp.y = y-(y0+1);
    v[3] = gradient4[gi15][0]*temp.x + gradient4[gi15][1]*temp.y + gradient4[gi15][2]*temp.z + gradient4[gi15][3]*temp.w;

    temp.x = x-x0;
    u[3] = gradient4[gi14][0]*temp.x + gradient4[gi14][1]*temp.y + gradient4[gi14][2]*temp.z + gradient4[gi14][3]*temp.w;

    Li1 = s[0] + Cx*(t[0]-s[0]);
    Li2 = u[0] + Cx*(v[0]-u[0]);
    Li3 = s[1] + Cx*(t[1]-s[1]);
    Li4 = u[1] + Cx*(v[1]-u[1]);
    Li5 = s[2] + Cx*(t[2]-s[2]);
    Li6 = u[2] + Cx*(v[2]-u[2]);
    Li7 = s[3] + Cx*(t[3]-s[3]);
    Li8 = u[3] + Cx*(v[3]-u[3]);

    Li9 = Li1 + Cy*(Li2-Li1);
    Li10 = Li3 + Cy*(Li4-Li3);
    Li11 = Li5 + Cy*(Li6-Li5);
    Li12 = Li7 + Cy*(Li8-Li7);

    Li13 = Li9 + Cz*(Li10-Li9);
    Li14 = Li11 + Cz*(Li12-Li11);

    return Li13 + Cw*(Li14-Li13);
}

#include <Nazara/Core/DebugOff.hpp>

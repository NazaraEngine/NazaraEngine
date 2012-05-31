// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

//#include <Nazara/Noise/Error.hpp>
//#include <Nazara/Noise/Config.hpp>
//#include <Nazara/Noise/Debug.hpp>

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
    nx = x/res;
    ny = y/res;
    nz = z/res;

    x0 = static_cast<int>(nx);
    y0 = static_cast<int>(ny);
    z0 = static_cast<int>(nz);

    ii = x0 & 255;
    jj = y0 & 255;
    kk = z0 & 255;

    gi0 = perm[ii + perm[jj + perm[kk]]] % 16;
    gi1 = perm[ii + 1 + perm[jj + perm[kk]]] % 16;
    gi2 = perm[ii + perm[jj + 1 + perm[kk]]] % 16;
    gi3 = perm[ii + 1 + perm[jj + 1 + perm[kk]]] % 16;

    gi4 = perm[ii + perm[jj + perm[kk + 1]]] % 16;
    gi5 = perm[ii + 1 + perm[jj + perm[kk + 1]]] % 16;
    gi6 = perm[ii + perm[jj + 1 + perm[kk + 1]]] % 16;
    gi7 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] % 16;

    temp.x = nx-x0;
    temp.y = ny-y0;
    temp.z = nz-z0;
    s[0] = gradient3[gi0][0]*temp.x + gradient3[gi0][1]*temp.y + gradient3[gi0][2]*temp.z;

    temp.x = nx-(x0+1);
    temp.y = ny-y0;
    t[0] = gradient3[gi1][0]*temp.x + gradient3[gi1][1]*temp.y + gradient3[gi1][2]*temp.z;

    temp.x = nx-x0;
    temp.y = ny-(y0+1);
    u[0] = gradient3[gi2][0]*temp.x + gradient3[gi2][1]*temp.y + gradient3[gi2][2]*temp.z;

    temp.x = nx-(x0+1);
    temp.y = ny-(y0+1);
    v[0] = gradient3[gi3][0]*temp.x + gradient3[gi3][1]*temp.y + gradient3[gi3][2]*temp.z;

    temp.x = nx-x0;
    temp.y = ny-y0;
    temp.z = nz-(z0+1);
    s[1] = gradient3[gi4][0]*temp.x + gradient3[gi4][1]*temp.y + gradient3[gi4][2]*temp.z;

    temp.x = nx-(x0+1);
    temp.y = ny-y0;
    t[1] = gradient3[gi5][0]*temp.x + gradient3[gi5][1]*temp.y + gradient3[gi5][2]*temp.z;

    temp.x = nx-x0;
    temp.y = ny-(y0+1);
    u[1] = gradient3[gi6][0]*temp.x + gradient3[gi6][1]*temp.y + gradient3[gi6][2]*temp.z;

    temp.x = nx-(x0+1);
    temp.y = ny-(y0+1);
    v[1] = gradient3[gi7][0]*temp.x + gradient3[gi7][1]*temp.y + gradient3[gi7][2]*temp.z;

    tmp = nx-x0;
    Cx = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    Li1 = s[0] + Cx*(t[0]-s[0]);
    Li2 = u[0] + Cx*(v[0]-u[0]);
    Li3 = s[1] + Cx*(t[1]-s[1]);
    Li4 = u[1] + Cx*(v[1]-u[1]);

    tmp = ny-y0;
    Cy = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    Li5 = Li1 + Cy*(Li2-Li1);
    Li6 = Li3 + Cy*(Li4-Li3);

    tmp = nz-z0;
    Cz = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    return Li5 + Cz*(Li6-Li5);
}

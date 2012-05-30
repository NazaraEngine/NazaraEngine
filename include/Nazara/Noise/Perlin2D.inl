// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

template <typename T>
NzPerlin2D<T>::NzPerlin2D()
{
    T unit = 1.0/sqrt(2);
    T grad2Temp[][2] = {{unit,unit},{-unit,unit},{unit,-unit},{-unit,-unit},
                            {1,0},{-1,0},{0,1},{0,-1}};

    for(int i(0) ; i < 8 ; ++i)
        for(int j(0) ; j < 2 ; ++j)
            gradient2[i][j] = grad2Temp[i][j];
}

template <typename T>
T NzPerlin2D<T>::GetValue(T x, T y, T res)
{
    nx = x/res;
    ny = y/res;

    x0 = static_cast<int>(nx);
    y0 = static_cast<int>(ny);

    ii = x0 & 255;
    jj = y0 & 255;

    gi0 = perm[ii + perm[jj]] & 7;
    gi1 = perm[ii + 1 + perm[jj]] & 7;
    gi2 = perm[ii + perm[jj + 1]] & 7;
    gi3 = perm[ii + 1 + perm[jj + 1]] & 7;

    temp.x = nx-x0;
    temp.y = ny-y0;
    s = gradient2[gi0][0]*temp.x + gradient2[gi0][1]*temp.y;

    temp.x = nx-(x0+1);
    temp.y = ny-y0;
    t = gradient2[gi1][0]*temp.x + gradient2[gi1][1]*temp.y;

    temp.x = nx-x0;
    temp.y = ny-(y0+1);
    u = gradient2[gi2][0]*temp.x + gradient2[gi2][1]*temp.y;

    temp.x = nx-(x0+1);
    temp.y = ny-(y0+1);
    v = gradient2[gi3][0]*temp.x + gradient2[gi3][1]*temp.y;

    tmp = nx-x0;
    Cx = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    Li1 = s + Cx*(t-s);
    Li2 = u + Cx*(v-u);

    tmp = ny - y0;
    Cy = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    return Li1 + Cy*(Li2-Li1);
}


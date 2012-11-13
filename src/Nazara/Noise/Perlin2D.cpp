// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Perlin2D.hpp>
#include <Nazara/Noise/Debug.hpp>

NzPerlin2D::NzPerlin2D()
{
    int grad2Temp[][2] = {{1,1},{-1,1},{1,-1},{-1,-1},
                        {1,0},{-1,0},{0,1},{0,-1}};

    for(int i(0) ; i < 8 ; ++i)
        for(int j(0) ; j < 2 ; ++j)
            gradient2[i][j] = grad2Temp[i][j];
}

NzPerlin2D::NzPerlin2D(int seed) : NzPerlin2D()
{
    this->SetNewSeed(seed);
    this->ShufflePermutationTable();
}

float NzPerlin2D::GetValue(float x, float y, float resolution)
{
    x *= resolution;
    y *= resolution;

    x0 = fastfloor(x);
    y0 = fastfloor(y);

    ii = x0 & 255;
    jj = y0 & 255;

    gi0 = perm[ii +     perm[jj]] & 7;
    gi1 = perm[ii + 1 + perm[jj]] & 7;
    gi2 = perm[ii +     perm[jj + 1]] & 7;
    gi3 = perm[ii + 1 + perm[jj + 1]] & 7;

    temp.x = x-x0;
    temp.y = y-y0;

    Cx = temp.x * temp.x * temp.x * (temp.x * (temp.x * 6 - 15) + 10);
    Cy = temp.y * temp.y * temp.y * (temp.y * (temp.y * 6 - 15) + 10);

    s = gradient2[gi0][0]*temp.x + gradient2[gi0][1]*temp.y;

    temp.x = x-(x0+1);
    t = gradient2[gi1][0]*temp.x + gradient2[gi1][1]*temp.y;

    temp.y = y-(y0+1);
    v = gradient2[gi3][0]*temp.x + gradient2[gi3][1]*temp.y;

    temp.x = x-x0;
    u = gradient2[gi2][0]*temp.x + gradient2[gi2][1]*temp.y;

    Li1 = s + Cx*(t-s);
    Li2 = u + Cx*(v-u);

    return Li1 + Cy*(Li2-Li1);
}

// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

//#include <Nazara/Noise/Error.hpp>
//#include <Nazara/Noise/Config.hpp>
//#include <Nazara/Noise/Debug.hpp>
#include <iostream>

template <typename T>
NzPerlin1D<T>::NzPerlin1D()
{
    gradient1[0] = 1;
    gradient1[1] = -1;
}

template <typename T>
T NzPerlin1D<T>::GetValue(T x, T res)
{
    nx = x/res;
    x0 = static_cast<int>(nx);
    ii = x0 & 255;

    gi0 = perm[ii] % 2;
    gi1 = perm[ii + 1] % 2;

    tmp = nx-x0;
    s = gradient1[gi0]*tmp;

    tmp = nx-(x0+1);
    t = gradient1[gi1]*tmp;

    tmp = nx-x0;
    Cx = tmp * tmp * tmp * (tmp * (tmp * 6 - 15) + 10);

    return s + Cx*(t-s);
}

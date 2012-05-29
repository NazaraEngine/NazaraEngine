// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include "NoiseBase.hpp"
//#include <Nazara/Noise/NoiseBase.hpp>
//#include <Nazara/Noise/Error.hpp>
//#include <Nazara/Noise/Config.hpp>
//#include <Nazara/Noise/Debug.hpp>

NzNoiseBase::NzNoiseBase(int seed)
{
    Ua = 16807;
    Uc = 0;
    Um = 2147483647;
    UcurrentSeed = 0;
    Uprevious = 0;

    SetNewSeed(seed);

    for(int i(0) ; i < 256 ; i++)
        perm[i] = i;

}

void NzNoiseBase::SetNewSeed(int seed)
{
    Uprevious = seed;
    UcurrentSeed = seed;
}

int NzNoiseBase::GetUniformRandomValue()
{
    Ulast = Ua*Uprevious + Uc%Um;
    Uprevious = Ulast;
    return Ulast;
}

void NzNoiseBase::ShufflePermutationTable()
{
    int xchanger;
    unsigned int ncase;

    for(int j(0) ; j < 10 ; ++j)
        for (int i(0); i < 256 ; ++i)
        {
            ncase = this->GetUniformRandomValue() & 255;
            xchanger = perm[i];
            perm[i] = perm[ncase];
            perm[ncase] = xchanger;
        }

    for(int i(256) ; i < 512; ++i)
        perm[i] = perm[i & 255];
}

NzNoiseBase::~NzNoiseBase()
{
    //dtor
}

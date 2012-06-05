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

    for(int j(0) ; j < 20 ; ++j)
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

int NzNoiseBase::fastfloor(float n)
{
    return (n >= 0) ? static_cast<int>(n) : static_cast<int>(n-1);
}

int NzNoiseBase::JenkinsHash(int a, int b, int c)
{
    a = a-b;  a = a - c;  a = a^(static_cast<unsigned int>(c) >> 13);
    b = b-c;  b = b - a;  b = b^(a << 8);
    c = c-a;  c = c - b;  c = c^(static_cast<unsigned int>(b) >> 13);
    a = a-b;  a = a - c;  a = a^(static_cast<unsigned int>(c) >> 12);
    b = b-c;  b = b - a;  b = b^(a << 16);
    c = c-a;  c = c - b;  c = c^(static_cast<unsigned int>(b) >> 5);
    a = a-b;  a = a - c;  a = a^(static_cast<unsigned int>(c) >> 3);
    b = b-c;  b = b - a;  b = b^(a << 10);
    c = c-a;  c = c - b;  c = c^(static_cast<unsigned int>(b) >> 15);
    return c;
}

/*
//key = 64 bits
public long hash64shift(long key)
{
  key = (~key) + (key << 21); // key = (key << 21) - key - 1;
  key = key ^ (key >>> 24);
  key = (key + (key << 3)) + (key << 8); // key * 265
  key = key ^ (key >>> 14);
  key = (key + (key << 2)) + (key << 4); // key * 21
  key = key ^ (key >>> 28);
  key = key + (key << 31);
  return key;
}*/

NzNoiseBase::~NzNoiseBase()
{
    //dtor
}

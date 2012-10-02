// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NOISEBASE_HPP
#define NOISEBASE_HPP

#include <Nazara/Prerequesites.hpp>

enum nzNoises
{
    PERLIN,
    SIMPLEX,
    CELL
};

class NAZARA_API NzNoiseBase
{
    public:
        NzNoiseBase(int seed = 0);
        ~NzNoiseBase() = default;

        void SetNewSeed(int seed);
        int GetUniformRandomValue();
        void ShufflePermutationTable();

        int fastfloor(float n);
        int JenkinsHash(int a, int b, int c);
    protected:
        int perm[512];
    private:
        int Ua, Uc, Um;
        int UcurrentSeed;
        int Uprevious, Ulast;

};

#endif // NOISEBASE_HPP

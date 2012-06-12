// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef COMPLEXNOISEBASE_HPP
#define COMPLEXNOISEBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>

class NAZARA_API NzComplexNoiseBase : public NzNoiseBase
{
    public:
        NzComplexNoiseBase();
        ~NzComplexNoiseBase() = default;

        void SetLacunarity(float lacunarity);
        void SetHurstParameter(float h);
        void SetOctavesNumber(float octaves);
        void RecomputeExponentArray();

    protected:
        float m_lacunarity;
        float m_hurst;
        float m_octaves;
        float exponent_array[30];
        float m_sum;
    private:
        bool m_parametersModified;

};

#endif // COMPLEXNOISEBASE_HPP

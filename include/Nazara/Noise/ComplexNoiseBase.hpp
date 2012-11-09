// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef COMPLEXNOISEBASE_HPP
#define COMPLEXNOISEBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <array>

class NAZARA_API NzComplexNoiseBase
{
    public:
        NzComplexNoiseBase();
        ~NzComplexNoiseBase() = default;

        const std::array<float, 30>& GetExponentArray() const; //For debug purpose
        float GetHurstParameter() const;
        float GetLacunarity() const;
        float GetOctaveNumber() const;
        void SetHurstParameter(float h);
        void SetLacunarity(float lacunarity);
        void SetOctavesNumber(float octaves);
        void RecomputeExponentArray();

    protected:
        float m_lacunarity;
        float m_hurst;
        float m_octaves;
        std::array<float, 30> m_exponent_array;
        float m_sum;
    private:
        bool m_parametersModified;
};

#endif // COMPLEXNOISEBASE_HPP

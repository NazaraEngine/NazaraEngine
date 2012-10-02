// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef COMPLEXNOISEBASE_HPP
#define COMPLEXNOISEBASE_HPP

#include <Nazara/Prerequesites.hpp>

template <typename T>
class NzComplexNoiseBase
{
    public:
        NzComplexNoiseBase();
        ~NzComplexNoiseBase() = default;

        T GetOctaveNumber() const;
        T GetLacunarity() const;
        T GetHurstParameter() const;
        void SetLacunarity(T lacunarity);
        void SetHurstParameter(T h);
        void SetOctavesNumber(T octaves);
        void RecomputeExponentArray();

    protected:
        T m_lacunarity;
        T m_hurst;
        T m_octaves;
        T exponent_array[30];
        T m_sum;
    private:
        bool m_parametersModified;

};

#include<Nazara/Noise/ComplexNoiseBase.inl>

#endif // COMPLEXNOISEBASE_HPP

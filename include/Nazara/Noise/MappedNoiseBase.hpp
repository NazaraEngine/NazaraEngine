// Copyright (C) 2014 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MAPPEDNOISEBASE_HPP
#define NAZARA_MAPPEDNOISEBASE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/NoiseBase.hpp>

class NAZARA_API NzMappedNoiseBase : public NzNoiseBase
{
    public:
        NzMappedNoiseBase();
        ~NzMappedNoiseBase() = default;

        float GetGain() const;
        float GetOffset() const;
        float GetResolution() const;
        void SetGain(float gain);
        void SetOffset(float offset);
        void SetResolution(float resolution);
    protected:
        float m_gain;
        float m_offset;
        float m_resolution;
};

#endif // NAZARA_MAPPEDNOISEBASE_HPP

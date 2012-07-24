// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MAPPEDNOISEBASE_HPP
#define NAZARA_MAPPEDNOISEBASE_HPP

#include <Nazara/Prerequesites.hpp>

template <typename T> class NzMappedNoiseBase
{
    public:
        NzMappedNoiseBase();
        ~NzMappedNoiseBase() = default;

        T GetGain() const;
        T GetOffset() const;
        T GetResolution() const;
        void SetGain(T gain);
        void SetOffset(T offset);
        void SetResolution(T resolution);
    protected:
        T m_gain;
        T m_offset;
        T m_resolution;
};

#include <Nazara/Noise/MappedNoiseBase.inl>

#endif // NAZARA_MAPPEDNOISEBASE_HPP

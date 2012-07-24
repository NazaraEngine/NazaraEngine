// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACT3DNOISE_HPP
#define NAZARA_ABSTRACT3DNOISE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/MappedNoiseBase.hpp>

template <typename T> class NAZARA_API NzAbstract3DNoise : public NzMappedNoiseBase
{
    public:
        virtual T GetMappedValue(T x, T y, T z);
        virtual T GetValue(T x, T y, T z) = 0;
        virtual T GetValue(T x, T y, T z, T resolution);
};

#include <Nazara/Noise/Abstract3DNoise.inl>

#endif // NAZARA_ABSTRACT3DNOISE_HPP

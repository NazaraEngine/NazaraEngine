// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACT4DNOISE_HPP
#define NAZARA_ABSTRACT4DNOISE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/MappedNoiseBase.hpp>

template <typename T> class NzAbstract4DNoise : public NzMappedNoiseBase<T>
{
    public:
        virtual T GetBasicValue(T x, T y, T z, T w);
        virtual T GetMappedValue(T x, T y, T z, T w);
        virtual T GetValue(T x, T y, T z, T w, T resolution) = 0;
};

#include <Nazara/Noise/Abstract4DNoise.inl>

#endif // NAZARA_ABSTRACT4DNOISE_HPP

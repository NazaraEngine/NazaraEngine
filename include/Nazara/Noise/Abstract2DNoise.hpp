// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACT2DNOISE_HPP
#define NAZARA_ABSTRACT2DNOISE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/MappedNoiseBase.hpp>

template <typename T> class NAZARA_API NzAbstract2DNoise : public NzMappedNoiseBase<T>
{
    public:
        virtual T GetMappedValue(T x, T y);
        virtual T GetValue(T x, T y, T resolution) = 0;
        virtual T GetValue(T x, T y);

};

#include <Nazara/Noise/Abstract2DNoise.inl>

#endif // NAZARA_ABSTRACT2DNOISE_HPP

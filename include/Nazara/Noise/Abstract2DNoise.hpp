// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACT2DNOISE_HPP
#define NAZARA_ABSTRACT2DNOISE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/MappedNoiseBase.hpp>

class NAZARA_API NzAbstract2DNoise : public NzMappedNoiseBase
{
    public:
        virtual ~NzAbstract2DNoise();

        float GetBasicValue(float x, float y);
        float GetMappedValue(float x, float y);
        virtual float GetValue(float x, float y, float resolution) = 0;
};

#endif // NAZARA_ABSTRACT2DNOISE_HPP

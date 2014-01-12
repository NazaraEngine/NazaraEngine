// Copyright (C) 2014 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACT4DNOISE_HPP
#define NAZARA_ABSTRACT4DNOISE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/MappedNoiseBase.hpp>

class NAZARA_API NzAbstract4DNoise : public NzMappedNoiseBase
{
    public:
        virtual ~NzAbstract4DNoise();

        float GetBasicValue(float x, float y, float z, float w);
        float GetMappedValue(float x, float y, float z, float w);
        virtual float GetValue(float x, float y, float z, float w, float resolution) = 0;
};

#endif // NAZARA_ABSTRACT4DNOISE_HPP

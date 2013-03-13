// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACT3DNOISE_HPP
#define NAZARA_ABSTRACT3DNOISE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Noise/MappedNoiseBase.hpp>

class NAZARA_API NzAbstract3DNoise : public NzMappedNoiseBase
{
    public:
        virtual ~NzAbstract3DNoise();

        float GetBasicValue(float x, float y, float z);
        float GetMappedValue(float x, float y, float z);
        virtual float GetValue(float x, float y, float z, float resolution) = 0;
};

#endif // NAZARA_ABSTRACT3DNOISE_HPP

// Copyright (C) 2014 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Abstract3DNoise.hpp>
#include <Nazara/Noise/Debug.hpp>

NzAbstract3DNoise::~NzAbstract3DNoise() = default;

float NzAbstract3DNoise::GetBasicValue(float x, float y, float z)
{
    return this->GetValue(x,y,z,m_resolution);
}

float NzAbstract3DNoise::GetMappedValue(float x, float y, float z)
{
    return (this->GetValue(x,y,z,m_resolution) + m_offset) * m_gain ;
}

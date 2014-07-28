// Copyright (C) 2014 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Abstract2DNoise.hpp>
#include <Nazara/Noise/Debug.hpp>

NzAbstract2DNoise::~NzAbstract2DNoise() = default;

float NzAbstract2DNoise::GetBasicValue(float x, float y)
{
    return this->GetValue(x,y,m_resolution);
}

float NzAbstract2DNoise::GetMappedValue(float x, float y)
{
    return (this->GetValue(x,y,m_resolution) + m_offset) * m_gain;
}

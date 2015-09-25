// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Abstract3DNoise.hpp>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	Abstract3DNoise::~Abstract3DNoise() = default;

	float Abstract3DNoise::GetBasicValue(float x, float y, float z)
	{
		return this->GetValue(x,y,z,m_resolution);
	}

	float Abstract3DNoise::GetMappedValue(float x, float y, float z)
	{
		return (this->GetValue(x,y,z,m_resolution) + m_offset) * m_gain ;
	}
}

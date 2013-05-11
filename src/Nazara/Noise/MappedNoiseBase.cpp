// Copyright (C) 2013 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <stdexcept>
#include <Nazara/Noise/MappedNoiseBase.hpp>
#include <Nazara/Noise/Debug.hpp>

NzMappedNoiseBase::NzMappedNoiseBase() : m_gain(1.f), m_offset(0.f), m_resolution(30.f)
{

}

float NzMappedNoiseBase::GetGain() const
{
    return m_gain;
}

float NzMappedNoiseBase::GetOffset() const
{
    return m_offset;
}

float NzMappedNoiseBase::GetResolution() const
{
    return m_resolution;
}

void NzMappedNoiseBase::SetGain(float gain)
{
    m_gain = gain;
}

void NzMappedNoiseBase::SetOffset(float offset)
{
    m_offset = offset;
}

void NzMappedNoiseBase::SetResolution(float resolution)
{
    if (NzNumberEquals(resolution, 0.f))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << " : resolution cannot be 0.0f";

		throw std::domain_error(ss.ToString());
	}
    m_resolution = resolution;
}

// Copyright (C) 2015 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Noise/Config.hpp>
#include <stdexcept>
#include <Nazara/Noise/MappedNoiseBase.hpp>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	MappedNoiseBase::MappedNoiseBase() : m_gain(1.f), m_offset(0.f), m_resolution(30.f)
	{

	}

	float MappedNoiseBase::GetGain() const
	{
		return m_gain;
	}

	float MappedNoiseBase::GetOffset() const
	{
		return m_offset;
	}

	float MappedNoiseBase::GetResolution() const
	{
		return m_resolution;
	}

	void MappedNoiseBase::SetGain(float gain)
	{
		m_gain = gain;
	}

	void MappedNoiseBase::SetOffset(float offset)
	{
		m_offset = offset;
	}

	void MappedNoiseBase::SetResolution(float resolution)
	{
		if (NumberEquals(resolution, 0.f))
		{
			StringStream ss;
			ss << __FILE__ << ':' << __LINE__ << " : resolution cannot be 0.0f";

			throw std::domain_error(ss.ToString());
		}
		m_resolution = resolution;
	}
}

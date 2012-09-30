// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Noise/Config.hpp>
#include <stdexcept>
#include <Nazara/Noise/Debug.hpp>


template <typename T>
NzMappedNoiseBase<T>::NzMappedNoiseBase() : m_gain(1), m_offset(0), m_resolution(30)
{

}

template <typename T>
T NzMappedNoiseBase<T>::GetGain() const
{
    return m_gain;
}

template <typename T>
T NzMappedNoiseBase<T>::GetOffset() const
{
    return m_offset;
}

template <typename T>
T NzMappedNoiseBase<T>::GetResolution() const
{
    return m_resolution;
}

template <typename T>
void NzMappedNoiseBase<T>::SetGain(T gain)
{
    m_gain = gain;
}

template <typename T>
void NzMappedNoiseBase<T>::SetOffset(T offset)
{
    m_offset = offset;
}

template <typename T>
void NzMappedNoiseBase<T>::SetResolution(T resolution)
{
    if (NzNumberEquals(resolution, static_cast<T>(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << " : resolution cannot be 0.0f";

		throw std::domain_error(ss.ToString());
	}
    m_resolution = resolution;
}

#include <Nazara/Core/DebugOff.hpp>

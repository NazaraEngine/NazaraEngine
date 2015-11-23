// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline Unserializer::Unserializer(Stream& stream)
	{
		m_unserializationContext.currentBitPos = 8;
		m_unserializationContext.endianness = Endianness_BigEndian;
		m_unserializationContext.stream = &stream;
	}

	inline Endianness Unserializer::GetDataEndianness() const
	{
		return m_unserializationContext.endianness;
	}

	inline Stream& Unserializer::GetStream() const
	{
		return *m_unserializationContext.stream;
	}

	inline void Unserializer::SetDataEndianness(Endianness endiannes)
	{
		m_unserializationContext.endianness = endiannes;
	}

	inline void Unserializer::SetStream(Stream& stream)
	{
		m_unserializationContext.stream = &stream;
	}

	template<typename T>
	Unserializer& Unserializer::operator>>(T& value)
	{
		if (!Unserialize(m_unserializationContext, &value))
			NazaraError("Failed to serialize value");

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>

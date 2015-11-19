// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline OutputStream::OutputStream() :
	Stream(OpenMode_Current)
	{
		m_serializationContext.stream = this;
	}

	inline OutputStream::OutputStream(const OutputStream& stream) :
	Stream(stream),
	m_serializationContext(stream.m_serializationContext)
	{
		m_serializationContext.stream = this;
	}

	inline OutputStream::OutputStream(OutputStream&& stream) noexcept :
	Stream(std::move(stream)),
	m_serializationContext(std::move(stream.m_serializationContext))
	{
		m_serializationContext.stream = this;
	}

	template<typename T>
	OutputStream& OutputStream::operator<<(const T& value)
	{
		m_serializationContext.endianness = m_dataEndianness; //< In case m_dataEndianness changed

		if (!Serialize(m_serializationContext, value))
			NazaraError("Failed to serialize value");

		return *this;
	}

	inline OutputStream& OutputStream::operator=(const OutputStream& stream)
	{
		Stream::operator=(stream);

		m_serializationContext = stream.m_serializationContext;
		m_serializationContext.stream = this;

		return *this;
	}

	inline OutputStream& OutputStream::operator=(OutputStream&& stream) noexcept
	{
		Stream::operator=(std::move(stream));

		m_serializationContext = std::move(stream.m_serializationContext);
		m_serializationContext.stream = this;

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>

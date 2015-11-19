// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline InputStream::InputStream() :
	Stream(OpenMode_Current)
	{
		m_unserializationContext.stream = this;
	}

	inline InputStream::InputStream(const InputStream& stream) :
	Stream(stream),
	m_unserializationContext(stream.m_unserializationContext)
	{
		m_unserializationContext.stream = this;
	}

	inline InputStream::InputStream(InputStream&& stream) noexcept :
	Stream(std::move(stream)),
	m_unserializationContext(std::move(stream.m_unserializationContext))
	{
		m_unserializationContext.stream = this;
	}

	template<typename T>
	InputStream& InputStream::operator>>(T& value)
	{
		m_unserializationContext.endianness = m_dataEndianness; //< In case m_dataEndianness changed

		if (!Unserialize(m_unserializationContext, &value))
			NazaraError("Failed to unserialize value");

		return *this;
	}

	inline InputStream& InputStream::operator=(const InputStream& stream)
	{
		Stream::operator=(stream);

		m_unserializationContext = stream.m_unserializationContext;
		m_unserializationContext.stream = this;

		return *this;
	}

	inline InputStream& InputStream::operator=(InputStream&& stream) noexcept
	{
		Stream::operator=(std::move(stream));

		m_unserializationContext = std::move(stream.m_unserializationContext);
		m_unserializationContext.stream = this;

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>

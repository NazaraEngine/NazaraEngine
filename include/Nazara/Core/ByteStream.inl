// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline ByteStream::ByteStream(Stream* stream)
	{
		m_context.stream = stream;
	}

	inline ByteStream::ByteStream(ByteStream&& stream) :
	m_ownedStream(std::move(stream.m_ownedStream)),
	m_context(stream.m_context)
	{
		stream.m_context.stream = nullptr;
	}

	inline ByteStream::~ByteStream()
	{
		if (!FlushBits())
			NazaraWarning("Failed to flush bits at serializer destruction");
	}

	inline Endianness ByteStream::GetDataEndianness() const
	{
		return m_context.endianness;
	}

	inline Nz::UInt64 ByteStream::GetSize() const
	{
		if (m_context.stream)
			return m_context.stream->GetSize();
		else
			return 0;
	}

	inline Stream* ByteStream::GetStream() const
	{
		return m_context.stream;
	}

	inline bool ByteStream::FlushBits()
	{
		if (!m_context.stream)
			return true;

		if (m_context.currentBitPos != 8)
		{
			m_context.currentBitPos = 8; //< To prevent Serialize to flush bits itself

			if (!Serialize<UInt8>(m_context, m_context.currentByte))
				return false;
		}

		return true;
	}

	inline std::size_t ByteStream::Read(void* ptr, std::size_t size)
	{
		if (!m_context.stream)
			OnEmptyStream();

		FlushBits();
		return m_context.stream->Read(ptr, size);
	}

	inline void ByteStream::SetDataEndianness(Endianness endiannes)
	{
		m_context.endianness = endiannes;
	}

	inline void ByteStream::SetStream(Stream* stream)
	{
		// We don't want to lose some bits..
		FlushBits();

		m_context.stream = stream;
		m_ownedStream.reset();
	}

	inline void ByteStream::Write(const void* data, std::size_t size)
	{
		if (!m_context.stream)
			OnEmptyStream();

		FlushBits();
		m_context.stream->Write(data, size);
	}

	template<typename T>
	ByteStream& ByteStream::operator>>(T& value)
	{
		if (!m_context.stream)
			OnEmptyStream();

		if (!Unserialize(m_context, &value))
			NazaraError("Failed to serialize value");

		return *this;
	}

	template<typename T>
	ByteStream& ByteStream::operator<<(const T& value)
	{
		if (!m_context.stream)
			OnEmptyStream();

		if (!Serialize(m_context, value))
			NazaraError("Failed to serialize value");

		return *this;
	}

	inline ByteStream& ByteStream::operator=(ByteStream&& stream)
	{
		m_context = stream.m_context;
		m_ownedStream = std::move(stream.m_ownedStream);
		
		stream.m_context.stream = nullptr;

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>

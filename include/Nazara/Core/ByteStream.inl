// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a ByteStream object with a stream
	*/

	inline ByteStream::ByteStream(Stream* stream)
	{
		m_context.stream = stream;
	}

	/*!
	* \brief Destructs the object and calls FlushBits
	*
	* \remark Produces a NazaraWarning if flush did not work
	*
	* \see FlushBits
	*/

	inline ByteStream::~ByteStream()
	{
		if (!FlushBits())
			NazaraWarning("Failed to flush bits at serializer destruction");
	}

	/*!
	* \brief Reset stream
	*/
	inline void ByteStream::ClearStream()
	{
		// We don't want to lose some bits..
		FlushBits();

		m_context.stream = nullptr;
		m_ownedStream.reset();
	}

	/*!
	* \brief Gets the stream endianness
	* \return Type of the endianness
	*/

	inline Endianness ByteStream::GetDataEndianness() const
	{
		return m_context.endianness;
	}

	/*!
	* \brief Gets the size of the byte stream
	* \return Size of the stream
	*/

	inline Nz::UInt64 ByteStream::GetSize() const
	{
		if (m_context.stream)
			return m_context.stream->GetSize();
		else
			return 0;
	}

	/*!
	* \brief Gets the internal stream
	* \return Internal stream
	*/

	inline Stream* ByteStream::GetStream() const
	{
		return m_context.stream;
	}

	/*!
	* \brief Flushes the stream
	* \return true if flushing is successful
	*/

	inline bool ByteStream::FlushBits()
	{
		if (!m_context.stream)
			return true;

		if (m_context.writeBitPos != 8)
		{
			m_context.writeBitPos = 8; //< To prevent Serialize to flush bits itself

			if (!Serialize(m_context, m_context.writeByte))
				return false;
		}

		return true;
	}

	/*!
	* \brief Reads data
	* \return Number of data read
	*
	* \param ptr Preallocated buffer to contain information read
	* \param size Size of the read and thus of the buffer
	*/

	inline std::size_t ByteStream::Read(void* ptr, std::size_t size)
	{
		if (!m_context.stream)
			OnEmptyStream();

		FlushBits();
		return m_context.stream->Read(ptr, size);
	}

	/*!
	* \brief Sets the stream endianness
	*
	* \param endiannes Type of the endianness
	*/

	inline void ByteStream::SetDataEndianness(Endianness endiannes)
	{
		m_context.endianness = endiannes;
	}

	/*!
	* \brief Changes stream
	*
	* \param stream Stream existing
	*/
	inline void ByteStream::SetStream(Stream* stream)
	{
		ClearStream();

		m_context.stream = stream;
	}

	/*!
	* \brief Writes data
	* \return Number of data written
	*
	* \param buffer Preallocated buffer containing information to write
	* \param size Size of the writting and thus of the buffer
	*
	* \remark Produces a NazaraAssert if buffer is nullptr
	*/

	inline std::size_t ByteStream::Write(const void* data, std::size_t size)
	{
		if (!m_context.stream)
			OnEmptyStream();

		FlushBits();
		return m_context.stream->Write(data, size);
	}

	/*!
	* \brief Outputs a data from the stream
	* \return A reference to this
	*
	* \param value Value to unserialize
	*
	* \remark Produces a NazaraError if unserialization failed
	*/

	template<typename T>
	ByteStream& ByteStream::operator>>(T& value)
	{
		if (!m_context.stream)
			OnEmptyStream();

		if (!Unserialize(m_context, &value))
			NazaraError("Failed to serialize value");

		return *this;
	}

	/*!
	* \brief Adds the data to the stream
	* \return A reference to this
	*
	* \param value Value to serialize
	*
	* \remark Produces a NazaraError if serialization failed
	*/

	template<typename T>
	ByteStream& ByteStream::operator<<(const T& value)
	{
		if (!m_context.stream)
			OnEmptyStream();

		if (!Serialize(m_context, value))
			NazaraError("Failed to serialize value");

		return *this;
	}
}

#include <Nazara/Core/DebugOff.hpp>

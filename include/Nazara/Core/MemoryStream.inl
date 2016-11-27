// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::MemoryStream
	* \brief Constructs a MemoryStream object by default
	*/
	inline MemoryStream::MemoryStream() :
	Stream(StreamOption_None, OpenMode_ReadWrite),
	m_pos(0)
	{
	}

	/*!
	* \brief Constructs a MemoryStream object with a byte array
	*
	* \param byteArray Bytes to stream
	* \param openMode Reading/writing mode for the stream
	*/
	inline MemoryStream::MemoryStream(ByteArray* byteArray, OpenModeFlags openMode) :
	MemoryStream()
	{
		SetBuffer(byteArray, openMode);
	}

	/*!
	* \brief Gets the internal buffer
	* \return Buffer of bytes
	*
	* \remark Produces a NazaraAssert if buffer is invalid
	*/
	inline ByteArray& MemoryStream::GetBuffer()
	{
		NazaraAssert(m_buffer, "Invalid buffer");

		return *m_buffer;
	}

	/*!
	* \brief Gets the internal buffer
	* \return Buffer of bytes
	*
	* \remark Produces a NazaraAssert if buffer is invalid
	*/
	inline const ByteArray& MemoryStream::GetBuffer() const
	{
		NazaraAssert(m_buffer, "Invalid buffer");

		return *m_buffer;
	}
}

#include <Nazara/Core/DebugOff.hpp>

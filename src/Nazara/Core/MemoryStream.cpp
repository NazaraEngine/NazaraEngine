// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/MemoryStream.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <algorithm>
#include <cstring>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::MemoryStream
	* \brief Core class that represents a stream of memory
	*/

	/*!
	* \brief Clears the content of the stream
	*/

	void MemoryStream::Clear()
	{
		m_buffer->Clear();
		m_pos = 0;
	}

	/*!
	* \brief Gets the size of the raw memory
	* \return Size of the memory
	*/

	UInt64 MemoryStream::GetSize() const
	{
		return m_buffer->GetSize();
	}

	/*!
	* \brief Sets the buffer for the memory stream
	*
	* \param byteArray Bytes to stream
	* \param openMode Reading/writing mode for the stream
	*
	* \remark Produces a NazaraAssert if byteArray is nullptr
	*/

	void MemoryStream::SetBuffer(ByteArray* byteArray, OpenModeFlags openMode)
	{
		NazaraAssertMsg(byteArray, "Invalid ByteArray");

		m_buffer = byteArray;
		m_openMode = openMode;
	}

	/*!
	* \brief Flushes the stream
	*/

	void MemoryStream::FlushStream()
	{
		// Nothing to flush
	}

	void* MemoryStream::GetMemoryMappedPointer() const
	{
		return m_buffer->GetBuffer();
	}

	/*!
	* \brief Reads blocks
	* \return Number of blocks read
	*
	* \param buffer Preallocated buffer to contain information read
	* \param size Size of the read and thus of the buffer
	*/

	std::size_t MemoryStream::ReadBlock(void* buffer, std::size_t size)
	{
		if (TestStreamEnd())
			return 0;

		std::size_t readSize = std::min<std::size_t>(size, static_cast<std::size_t>(m_buffer->GetSize() - m_pos));

		if (buffer)
			std::memcpy(buffer, m_buffer->GetConstBuffer() + m_pos, readSize);

		m_pos += readSize;
		return readSize;
	}

	/*!
	* \brief Sets the position of the cursor
	* \return true
	*
	* \param offset Offset according to the beginning of the stream
	*/
	bool MemoryStream::SeekStreamCursor(UInt64 offset)
	{
		m_pos = offset;

		return true;
	}

	/*!
	* \brief Gets the position of the cursor
	* \return Position of the cursor
	*/
	UInt64 MemoryStream::TellStreamCursor() const
	{
		return m_pos;
	}

	/*!
	* \brief Checks whether the stream reached the end of the stream
	* \return true if cursor is at the end of the stream
	*/
	bool MemoryStream::TestStreamEnd() const
	{
		return m_pos >= m_buffer->size();
	}

	/*!
	* \brief Writes blocks
	* \return Number of blocks written
	*
	* \param buffer Preallocated buffer containing information to write
	* \param size Size of the writting and thus of the buffer
	*
	* \remark Produces a NazaraAssert if buffer is nullptr
	*/
	std::size_t MemoryStream::WriteBlock(const void* buffer, std::size_t size)
	{
		if (size > 0)
		{
			std::size_t endPos = static_cast<std::size_t>(m_pos + size);
			if (endPos > m_buffer->GetSize())
				m_buffer->Resize(endPos);

			NazaraAssertMsg(buffer, "Invalid buffer");

			std::memcpy(m_buffer->GetBuffer() + m_pos, buffer, size);

			m_pos = endPos;
		}

		return size;
	}
}

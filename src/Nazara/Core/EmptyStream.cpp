// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/EmptyStream.hpp>
#include <algorithm>
#include <cstring>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::EmptyStream
	* \brief Core class that simulate an empty buffer (discarding all content) which can be used to measure size
	*/

	/*!
	* \brief Resets the stream size to zero
	*/
	void EmptyStream::Clear()
	{
		m_size = 0;
	}

	/*!
	* \brief Gets the size of the raw memory (how many bytes would have been written on a regular stream)
	* \return Size occupied until now
	*/
	UInt64 EmptyStream::GetSize() const
	{
		return m_size;
	}

	/*!
	* \brief Flushes the stream (does nothing)
	*/
	void EmptyStream::FlushStream()
	{
		// Nothing to flush
	}

	/*!
	* \brief Reads data
	* \return Number of byte read (always zero)
	*
	* Reading from an empty stream does nothing and will always returns zero
	*
	* \param buffer Preallocated buffer to contain information read
	* \param size Size of the read and thus of the buffer
	*/
	std::size_t EmptyStream::ReadBlock(void* /*buffer*/, std::size_t /*size*/)
	{
		return 0;
	}

	/*!
	* \brief Does nothing
	* \return true
	*
	* \param offset Offset according to the beginning of the stream
	*/
	bool EmptyStream::SeekStreamCursor(UInt64 /*offset*/)
	{
		return true;
	}

	/*!
	* \brief Gets the position of the cursor (which is always zero)
	* \return Always zero
	*/
	UInt64 EmptyStream::TellStreamCursor() const
	{
		return 0;
	}

	/*!
	* \brief Checks whether the stream reached the end of the stream
	* \return Always false
	*/
	bool EmptyStream::TestStreamEnd() const
	{
		return false;
	}

	/*!
	* \brief Writes data
	* \return size
	*
	* \param buffer Dummy parameter
	* \param size How many bytes will be "written"
	*/
	std::size_t EmptyStream::WriteBlock(const void* /*buffer*/, std::size_t size)
	{
		m_size += size;

		return size;
	}
}

// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/MemoryView.hpp>
#include <algorithm>
#include <cstring>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::MemoryView
	* \brief Core class that represents a view of the memory behaving like a stream
	*/

	/*!
	* \brief Constructs a MemoryView object with a raw memory and a size
	*
	* \param ptr Pointer to raw memory
	* \param size Size that can be read
	*
	* \remark If preallocated space of ptr is less than the size, the behaviour is undefined
	*/

	MemoryView::MemoryView(void* ptr, UInt64 size) :
	Stream(StreamOption::None, OpenMode_ReadWrite),
	m_ptr(static_cast<UInt8*>(ptr)),
	m_pos(0),
	m_size(size)
	{
	}

	/*!
	* \brief Constructs a MemoryView object with a raw memory and a size
	*
	* \param ptr Constant pointer to raw memory
	* \param size Size that can be read
	*
	* \remark If preallocated space of ptr is less than the size, the behaviour is undefined
	*/

	MemoryView::MemoryView(const void* ptr, UInt64 size) :
	Stream(StreamOption::MemoryMapped, OpenMode::Read),
	m_ptr(static_cast<UInt8*>(const_cast<void*>(ptr))), //< Okay, right, const_cast is bad, but this pointer is still read-only
	m_pos(0),
	m_size(size)
	{
	}

	/*!
	* \brief Flushes the stream
	*/
	void MemoryView::FlushStream()
	{
		// Nothing to do
	}

	void* MemoryView::GetMemoryMappedPointer() const
	{
		return m_ptr;
	}

	/*!
	* \brief Gets the size of the raw memory
	* \return Size of the memory
	*/

	UInt64 MemoryView::GetSize() const
	{
		return m_size;
	}

	/*!
	* \brief Reads blocks
	* \return Number of blocks read
	*
	* \param buffer Preallocated buffer to contain information read
	* \param size Size of the read and thus of the buffer
	*/

	std::size_t MemoryView::ReadBlock(void* buffer, std::size_t size)
	{
		std::size_t readSize = std::min<std::size_t>(size, static_cast<std::size_t>(m_size - m_pos));

		if (buffer)
			std::memcpy(buffer, &m_ptr[m_pos], readSize);

		m_pos += readSize;
		return readSize;
	}

	/*!
	* \brief Sets the position of the cursor
	* \return true
	*
	* \param offset Offset according to the beginning of the stream
	*/
	bool MemoryView::SeekStreamCursor(UInt64 offset)
	{
		m_pos = std::min(offset, m_size);

		return true;
	}

	/*!
	* \brief Checks whether the stream reached the end of the stream
	* \return true if cursor is at the end of the stream
	*/
	bool MemoryView::TestStreamEnd() const
	{
		return m_pos >= m_size;
	}

	/*!
	* \brief Gets the position of the cursor
	* \return Position of the cursor
	*/
	UInt64 MemoryView::TellStreamCursor() const
	{
		return m_pos;
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

	std::size_t MemoryView::WriteBlock(const void* buffer, std::size_t size)
	{
		std::size_t endPos = static_cast<std::size_t>(m_pos + size);
		if (endPos > m_size)
			size = static_cast<std::size_t>(m_size - m_pos);

		NazaraAssertMsg(buffer, "Invalid buffer");

		std::memcpy(&m_ptr[m_pos], buffer, size);

		m_pos += size;
		return size;
	}
}

// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \brief Constructs a Stream object with options
	*
	* \param streamOptions Options for the stream
	* \param openMode Reading/writing mode for the stream
	*/

	inline Stream::Stream(StreamOptionFlags streamOptions, OpenModeFlags openMode) :
	m_bufferCapacity(0),
	m_bufferOffset(0),
	m_bufferSize(0),
	m_openMode(openMode),
	m_streamOptions(streamOptions),
	m_bufferCursor(0)
	{
	}

	/*!
	* \brief Enables the text mode
	*
	* \param textMode Enables the mode or disables
	*/

	inline void Stream::EnableBuffering(bool buffering, std::size_t bufferSize)
	{
		if (buffering)
		{
			m_streamOptions &= ~StreamOption::Unbuffered;
			if (m_bufferCapacity != bufferSize)
			{
				m_buffer = std::make_unique<UInt8[]>(bufferSize);
				m_bufferCursor = TellStreamCursor();
				m_bufferOffset = 0;
				m_bufferSize = 0;
				m_bufferCapacity = bufferSize;
			}
		}
		else
		{
			m_streamOptions |= StreamOption::Unbuffered;
			m_buffer.reset();
			m_bufferCapacity = 0;
		}
	}

	inline void Stream::EnableTextMode(bool textMode)
	{
		if (textMode)
			m_streamOptions |= StreamOption::Text;
		else
			m_streamOptions &= ~StreamOption::Text;
	}

	/*!
	* \brief Flushes the stream
	*
	* \remark Produces a NazaraAssert if file is not writable
	*/

	inline void Stream::Flush()
	{
		NazaraAssert(IsWritable(), "Stream is not writable");

		FlushStream();
	}

	/*!
	* \brief Gets the open mode of the stream
	* \return Reading/writing mode for the stream
	*/

	inline OpenModeFlags Stream::GetOpenMode() const
	{
		return m_openMode;
	}

	/*!
	* \brief Gets the options of the stream
	* \return Options of the stream
	*/

	inline StreamOptionFlags Stream::GetStreamOptions() const
	{
		return m_streamOptions;
	}

	/*!
	* \brief Checks whether the stream is readable
	* \return true if it is the case
	*/

	inline bool Stream::IsBufferingEnabled() const
	{
		return (m_streamOptions & StreamOption::Unbuffered) == 0;
	}

	inline bool Stream::IsReadable() const
	{
		return (m_openMode & OpenMode::ReadOnly) != 0;
	}

	/*!
	* \brief Checks whether the stream is sequential
	* \return true if it is the case
	*/

	inline bool Stream::IsSequential() const
	{
		return (m_streamOptions & StreamOption::Sequential) != 0;
	}

	/*!
	* \brief Checks whether the stream has text mode enabled
	* \return true if it is the case
	*/

	inline bool Stream::IsTextModeEnabled() const
	{
		return (m_streamOptions & StreamOption::Text) != 0;
	}

	/*!
	* \brief Checks whether the stream can be written
	* \return true if it is the case
	*/

	inline bool Stream::IsWritable() const
	{
		return (m_openMode & OpenMode::WriteOnly) != 0;
	}

	inline bool Stream::SetCursorPos(UInt64 offset)
	{
		if (m_bufferCapacity == 0)
			return SeekStreamCursor(offset);
		else
		{
			if (offset >= m_bufferCursor && offset - m_bufferCursor < m_bufferSize)
				m_bufferOffset += offset - m_bufferCursor;
			else
			{
				// Out of buffer
				if (!SeekStreamCursor(offset))
					return false;

				m_bufferCursor = offset;
				m_bufferOffset = 0;
				m_bufferSize = 0;
			}

			return true;
		}
	}

	/*!
	* \brief Writes in the stream the content of a buffer
	* \return Size of the writing
	*
	* \param buffer Buffer to get data from
	* \param size Size meant to be written
	*
	* \remark Produces a NazaraAssert if stream is not writable
	* \remark If preallocated space of buffer is less than the size, the behavior is undefined
	*/
	inline std::size_t Stream::Write(const void* buffer, std::size_t size)
	{
		NazaraAssert(IsWritable(), "Stream is not writable");

		std::size_t writeSize = WriteBlock(buffer, size);

		// For now, don't buffer writes
		m_bufferCursor += writeSize;
		m_bufferSize = 0;

		return writeSize;
	}
}

#include <Nazara/Core/DebugOff.hpp>

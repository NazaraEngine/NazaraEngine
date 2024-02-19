// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <cassert>

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
			m_streamOptions.Clear(StreamOption::Unbuffered);
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
			m_streamOptions.Set(StreamOption::Unbuffered);
			m_buffer.reset();
			m_bufferCapacity = 0;
		}
	}

	inline void Stream::EnableTextMode(bool textMode)
	{
		if (textMode)
			m_streamOptions.Set(StreamOption::Text);
		else
			m_streamOptions.Clear(StreamOption::Text);
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

	inline const void* Stream::GetMappedPointer() const
	{
		NazaraAssert(IsMemoryMapped(), "Stream is not memory-mapped");
		NazaraAssert(IsReadable(), "Stream is not readable");
		return GetMemoryMappedPointer();
	}

	inline void* Stream::GetMappedPointerMutable()
	{
		NazaraAssert(IsMemoryMapped(), "Stream is not memory-mapped");
		NazaraAssert(IsWritable(), "Stream is not writable");
		return GetMemoryMappedPointer();
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
	* \brief Reads a line from the stream
	*
	* Reads the stream until a line separator or the end of the stream is found.
	*
	* If lineSize does not equal zero, it represents the maximum character count to be read from the stream.
	*
	* \param lineSize Maximum number of characters to read, or zero for no limit
	*
	* \return Line read from file
	*
	* \remark With the text stream option, "\r\n" is treated as "\n"
	* \remark The line separator character is not returned as part of the string
	*/
	inline std::string Stream::ReadLine(unsigned int lineSize)
	{
		std::string line;
		ReadLine(line, lineSize);
		return line;
	}

	/*!
	* \brief Checks whether the stream is readable
	* \return true if it is the case
	*/
	inline bool Stream::IsBufferingEnabled() const
	{
		return m_streamOptions.Test(StreamOption::Unbuffered);
	}

	inline bool Stream::IsMemoryMapped() const
	{
		return m_streamOptions.Test(StreamOption::MemoryMapped);
	}

	inline bool Stream::IsReadable() const
	{
		return m_openMode.Test(OpenMode::Read);
	}

	/*!
	* \brief Checks whether the stream is sequential
	* \return true if it is the case
	*/
	inline bool Stream::IsSequential() const
	{
		return m_streamOptions.Test(StreamOption::Sequential);
	}

	/*!
	* \brief Checks whether the stream has text mode enabled
	* \return true if it is the case
	*/
	inline bool Stream::IsTextModeEnabled() const
	{
		return m_streamOptions.Test(StreamOption::Text);
	}

	/*!
	* \brief Checks whether the stream can be written
	* \return true if it is the case
	*/
	inline bool Stream::IsWritable() const
	{
		return m_openMode.Test(OpenMode::Write);
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


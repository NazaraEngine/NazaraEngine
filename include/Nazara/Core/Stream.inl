// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include "Stream.hpp"

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
	m_openMode(openMode),
	m_streamOptions(streamOptions)
	{
	}

	/*!
	* \brief Enables the text mode
	*
	* \param textMode Enables the mode or disables
	*/

	inline void Stream::EnableTextMode(bool textMode)
	{
		if (textMode)
			m_streamOptions |= StreamOption_Text;
		else
			m_streamOptions &= ~StreamOption_Text;
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

	inline bool Stream::IsReadable() const
	{
		return (m_openMode & OpenMode_ReadOnly) != 0;
	}

	/*!
	* \brief Checks whether the stream is sequential
	* \return true if it is the case
	*/

	inline bool Stream::IsSequential() const
	{
		return (m_streamOptions & StreamOption_Sequential) != 0;
	}

	/*!
	* \brief Checks whether the stream has text mode enabled
	* \return true if it is the case
	*/

	inline bool Stream::IsTextModeEnabled() const
	{
		return (m_streamOptions & StreamOption_Text) != 0;
	}

	/*!
	* \brief Checks whether the stream can be written
	* \return true if it is the case
	*/

	inline bool Stream::IsWritable() const
	{
		return (m_openMode & OpenMode_WriteOnly) != 0;
	}

	/*!
	* \brief Reads the stream and puts the result in a buffer
	* \return Size of the read
	*
	* \param buffer Buffer to stock data
	* \param size Size meant to be read
	*
	* \remark Produces a NazaraAssert if stream is not readable
	* \remark If preallocated space of buffer is less than the size, the behavior is undefined
	*/

	inline std::size_t Stream::Read(void* buffer, std::size_t size)
	{
		NazaraAssert(IsReadable(), "Stream is not readable");

		return ReadBlock(buffer, size);
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

		return WriteBlock(buffer, size);
	}
}

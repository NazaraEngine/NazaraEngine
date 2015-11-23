// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include "Stream.hpp"

namespace Nz
{
	inline Stream::Stream(UInt32 streamOptions, UInt32 openMode) :
	m_openMode(openMode),
	m_streamOptions(streamOptions)
	{
	}

	inline void Stream::EnableTextMode(bool textMode)
	{
		if (textMode)
			m_streamOptions |= StreamOption_Text;
		else
			m_streamOptions &= ~StreamOption_Text;
	}

	inline void Stream::Flush()
	{
		NazaraAssert(IsWritable(), "Stream is not writable");

		FlushStream();
	}

	inline UInt32 Stream::GetOpenMode() const
	{
		return m_openMode;
	}

	inline UInt32 Stream::GetStreamOptions() const
	{
		return m_streamOptions;
	}

	inline bool Stream::IsReadable() const
	{
		return (m_openMode & OpenMode_ReadOnly) != 0;
	}

	inline bool Stream::IsSequential() const
	{
		return (m_streamOptions & StreamOption_Sequential) != 0;
	}

	inline bool Stream::IsTextModeEnabled() const
	{
		return (m_streamOptions & StreamOption_Text) != 0;
	}

	inline bool Stream::IsWritable() const
	{
		return (m_openMode & OpenMode_WriteOnly) != 0;
	}

	inline std::size_t Stream::Read(void* buffer, std::size_t size)
	{
		NazaraAssert(IsReadable(), "Stream is not readable");

		return ReadBlock(buffer, size);
	}

	inline std::size_t Stream::Write(const void* buffer, std::size_t size)
	{
		NazaraAssert(IsWritable(), "Stream is not writable");

		return WriteBlock(buffer, size);
	}
}

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

namespace Nz
{
	inline Stream::Stream(UInt32 openMode) :
	m_openMode(openMode),
	m_streamOptions(0)
	{
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
		return m_openMode & OpenMode_ReadOnly || m_openMode & OpenMode_ReadWrite;
	}

	inline bool Stream::IsWritable() const
	{
		return m_openMode & OpenMode_ReadWrite || m_openMode & OpenMode_WriteOnly;
	}

	inline void Stream::SetStreamOptions(UInt32 options)
	{
		m_streamOptions = options;
	}
}

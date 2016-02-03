// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline MemoryStream::MemoryStream() :
	Stream(StreamOption_None, OpenMode_ReadWrite),
	m_pos(0)
	{
	}

	inline MemoryStream::MemoryStream(ByteArray* byteArray, UInt32 openMode) :
	MemoryStream()
	{
		SetBuffer(byteArray, openMode);
	}

	inline ByteArray& MemoryStream::GetBuffer()
	{
		NazaraAssert(m_buffer, "Invalid buffer");

		return *m_buffer;
	}

	inline const ByteArray& MemoryStream::GetBuffer() const
	{
		NazaraAssert(m_buffer, "Invalid buffer");

		return *m_buffer;
	}
}

#include <Nazara/Core/DebugOff.hpp>

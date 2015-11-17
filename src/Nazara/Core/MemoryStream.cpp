// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MemoryStream.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	MemoryStream::MemoryStream(const void* ptr, UInt64 size) :
	Stream(OpenMode_ReadOnly),
	m_ptr(reinterpret_cast<const UInt8*>(ptr)),
	m_pos(0),
	m_size(size)
	{
	}

	MemoryStream::~MemoryStream()
	{
	}

	bool MemoryStream::EndOfStream() const
	{
		return m_pos == m_size;
	}

	UInt64 MemoryStream::GetCursorPos() const
	{
		return m_pos;
	}

	UInt64 MemoryStream::GetSize() const
	{
		return m_size;
	}

	std::size_t MemoryStream::Read(void* buffer, std::size_t size)
	{
		unsigned int readSize = std::min(static_cast<unsigned int>(size), static_cast<unsigned int>(m_size-m_pos));

		if (buffer)
			std::memcpy(buffer, &m_ptr[m_pos], readSize);

		m_pos += readSize;

		return readSize;
	}

	bool MemoryStream::SetCursorPos(UInt64 offset)
	{
		m_pos = std::min(offset, m_size);

		return true;
	}
}

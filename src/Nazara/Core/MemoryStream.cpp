// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MemoryStream.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	void MemoryStream::Clear()
	{
		m_buffer->Clear();
		m_pos = 0;
	}

	bool MemoryStream::EndOfStream() const
	{
		return m_pos >= m_buffer->size();
	}

	UInt64 MemoryStream::GetCursorPos() const
	{
		return m_pos;
	}

	UInt64 MemoryStream::GetSize() const
	{
		return m_buffer->GetSize();
	}

	void MemoryStream::SetBuffer(ByteArray* byteArray, UInt32 openMode)
	{
		m_buffer = byteArray;
		m_openMode = openMode;
	}

	bool MemoryStream::SetCursorPos(UInt64 offset)
	{
		m_pos = offset;

		return true;
	}

	void MemoryStream::FlushStream()
	{
		// Nothing to flush
	}

	std::size_t MemoryStream::ReadBlock(void* buffer, std::size_t size)
	{
		if (EndOfStream())
			return 0;

		std::size_t readSize = std::min<std::size_t>(size, static_cast<std::size_t>(m_buffer->GetSize() - m_pos));

		if (buffer)
			std::memcpy(buffer, m_buffer->GetConstBuffer() + m_pos, readSize);

		m_pos += readSize;
		return readSize;
	}

	std::size_t MemoryStream::WriteBlock(const void* buffer, std::size_t size)
	{
		std::size_t endPos = static_cast<std::size_t>(m_pos + size);
		if (endPos > m_buffer->GetSize())
			m_buffer->Resize(endPos);

		std::memcpy(m_buffer->GetBuffer() + m_pos, buffer, size);
		
		m_pos = endPos;
		return size;
	}
}

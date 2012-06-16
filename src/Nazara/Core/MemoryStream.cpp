// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/MemoryStream.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

NzMemoryStream::NzMemoryStream(const void* ptr, nzUInt64 size) :
m_ptr(reinterpret_cast<const nzUInt8*>(ptr)),
m_pos(0),
m_size(size)
{
}

NzMemoryStream::~NzMemoryStream()
{
}

bool NzMemoryStream::EndOfStream() const
{
	return m_pos == m_size;
}

nzUInt64 NzMemoryStream::GetCursorPos() const
{
	return m_pos;
}

nzUInt64 NzMemoryStream::GetSize() const
{
	return m_size;
}

std::size_t NzMemoryStream::Read(void* buffer, std::size_t size)
{
	unsigned int readSize = std::min(static_cast<nzUInt64>(size), m_size-m_pos);
	std::memcpy(buffer, &m_ptr[m_pos], readSize);
	m_pos += readSize;

	return readSize;
}

bool NzMemoryStream::SetCursorPos(nzUInt64 offset)
{
	m_pos = std::min(offset, m_size);

	return true;
}

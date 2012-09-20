// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

/*
#include <Nazara/Core/ByteArray.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

inline unsigned int nzPow2(unsigned int n)
{
	unsigned int x = 1;

	while(x <= n)
		x <<= 1;

	return x;
}
// Cet algorithme est inspiré de la documentation de Qt
inline unsigned int nzGetNewSize(unsigned int newSize)
{
	if (newSize < 20)
		return newSize+4;
	else
	{
		if (newSize < (1 << 12)-12)
			return nzPow2(newSize << 1)-12;
		else
			return newSize + (1 << 11);
	}
}

NzByteArray::NzByteArray() :
m_sharedArray(&emptyArray)
{
}

NzByteArray::NzByteArray(const nzUInt8* buffer, unsigned int bufferLength)
{
	if (bufferLength > 0)
	{
		m_sharedArray = new SharedArray;
		m_sharedArray->buffer = new nzUInt8[bufferLength];
		m_sharedArray->capacity = bufferLength;
		m_sharedArray->size = bufferLength;
		std::memcpy(m_sharedArray->buffer, buffer, bufferLength);
	}
	else
		m_sharedArray = &emptyArray;
}

NzByteArray::NzByteArray(const NzByteArray& buffer) :
m_sharedArray(buffer.m_sharedArray)
{
	if (m_sharedArray != &emptyArray)
	{
		NazaraMutexLock(m_sharedArray->mutex);
		m_sharedArray->refCount++;
		NazaraMutexUnlock(m_sharedArray->mutex);
	}
}

NzByteArray::NzByteArray(NzByteArray&& buffer) noexcept :
m_sharedArray(buffer.m_sharedArray)
{
	buffer.m_sharedArray = &emptyArray;
}

NzByteArray::NzByteArray(SharedArray* sharedArray) :
m_sharedArray(sharedArray)
{
}

NzByteArray::~NzByteArray()
{
	ReleaseArray();
}

NzByteArray& NzByteArray::Append(const NzByteArray& byteArray)
{
	if (byteArray.m_sharedArray->size == 0)
		return *this;

	if (m_sharedArray->size == 0 && m_sharedArray->capacity < byteArray.m_sharedArray->size)
		return operator=(byteArray);

	if (m_sharedArray->capacity >= m_sharedArray->size + byteArray.m_sharedArray->size)
	{
		EnsureOwnership();

		std::memcpy(&m_sharedArray->buffer[m_sharedArray->size], byteArray.m_sharedArray->buffer, byteArray.m_sharedArray->size);
		m_sharedArray->size += byteArray.m_sharedArray->size;
	}
	else
	{
		unsigned int newSize = m_sharedArray->size + byteArray.m_sharedArray->size;
		unsigned int bufferSize = nzGetNewSize(newSize);

		nzUInt8* buffer = new nzUInt8[bufferSize+1];
		std::memcpy(buffer, m_sharedArray->buffer, m_sharedArray->size);
		std::memcpy(&buffer[m_sharedArray->size], byteArray.m_sharedArray->buffer, byteArray.m_sharedArray->size);

		ReleaseArray();
		m_sharedArray = new SharedArray;
		m_sharedArray->buffer = buffer;
		m_sharedArray->capacity = bufferSize;
		m_sharedArray->size = newSize;
	}

	return *this;
}

void NzByteArray::Clear()
{
	ReleaseArray();
}

nzUInt8* NzByteArray::GetBuffer()
{
	EnsureOwnership();

	return m_sharedArray->buffer;
}

unsigned int NzByteArray::GetCapacity() const
{
	return m_sharedArray->capacity;
}

const nzUInt8* NzByteArray::GetConstBuffer() const
{
	return m_sharedArray->buffer;
}

unsigned int NzByteArray::GetSize() const
{
	return m_sharedArray->size;
}

NzByteArray& NzByteArray::Insert(int pos, const nzUInt8* buffer, unsigned int bufferLength)
{
	if (bufferLength == 0)
		return *this;

	if (m_sharedArray->size == 0)
		return operator=(string);

	if (pos < 0)
		pos = std::max(static_cast<int>(m_sharedArray->size + pos), 0);

	unsigned int start = std::min(static_cast<unsigned int>(pos), m_sharedArray->size);

	// Si le buffer est déjà suffisamment grand
	if (m_sharedArray->capacity >= m_sharedArray->size + bufferLength)
	{
		EnsureOwnership();

		std::memmove(&m_sharedArray->buffer[start+bufferLength], &m_sharedArray->buffer[start], m_sharedArray->size);
		std::memcpy(&m_sharedArray->buffer[start], buffer, bufferLength);

		m_sharedArray->size += bufferLength;
	}
	else
	{
		unsigned int newSize = m_sharedArray->size+bufferLength;
		nzUInt8* newBuffer = new nzUInt8[newSize+1];

		nzUInt8* ptr = newBuffer;
		const nzUInt8* s = m_sharedArray->buffer;

		while (ptr != &newBuffer[start])
			*ptr++ = *s++;

		while (ptr != &newBuffer[start+bufferLength])
			*ptr++ = *buffer++;

		std::strcpy(ptr, s);

		ReleaseString();
		m_sharedArray = new SharedString;
		m_sharedArray->allocatedSize = newSize;
		m_sharedArray->buffer = newBuffer;
		m_sharedArray->size = newSize;
	}

	return *this;
}

NzByteArray& NzByteArray::Insert(int pos, const NzByteArray& byteArray)
{
	if (string.m_sharedArray->size == 0)
		return *this;

	if (m_sharedArray->size == 0 && m_sharedArray->capacity < string.m_sharedArray->size)
		return operator=(string);

	if (pos < 0)
		pos = std::max(static_cast<int>(m_sharedArray->size + pos), 0);

	unsigned int start = std::min(static_cast<unsigned int>(pos), m_sharedArray->size);

	// Si le buffer est déjà suffisamment grand
	if (m_sharedArray->capacity >= m_sharedArray->size + string.m_sharedArray->size)
	{
		EnsureOwnership();

		std::memmove(&m_sharedArray->string[start+string.m_sharedArray->size], &m_sharedArray->string[start], m_sharedArray->size*sizeof(char));
		std::memcpy(&m_sharedArray->string[start], string.m_sharedArray->string, string.m_sharedArray->size*sizeof(char));

		m_sharedArray->size += string.m_sharedArray->size;
	}
	else
	{
		unsigned int newSize = m_sharedArray->size+string.m_sharedArray->size;
		char* newString = new char[newSize+1];

		char* ptr = newString;
		const char* s = m_sharedArray->string;

		while (ptr != &newString[start])
			*ptr++ = *s++;

		const char* p = string.m_sharedArray->string;
		while (ptr != &newString[start+string.m_sharedArray->size])
			*ptr++ = *p++;

		std::strcpy(ptr, s);

		ReleaseString();
		m_sharedArray = new SharedString;
		m_sharedArray->capacity = newSize;
		m_sharedArray->size = newSize;
		m_sharedArray->string = newString;
	}

	return *this;
}

bool NzByteArray::IsEmpty() const
{
	return m_sharedArray->size == 0;
}

void NzByteArray::Reserve(unsigned int bufferSize)
{
	if (m_sharedArray->allocatedSize >= bufferSize)
		return;

	nzUInt8* ptr = new nzUInt8[bufferSize+1];
	if (m_sharedArray->size > 0)
		std::memcpy(ptr, m_sharedArray->buffer, m_sharedArray->size);

	unsigned int size = m_sharedArray->size;

	ReleaseArray();
	m_sharedArray = new SharedString;
	m_sharedArray->allocatedSize = bufferSize;
	m_sharedArray->buffer = ptr;
	m_sharedArray->size = size;
}

NzByteArray::SharedString NzByteArray::emptyArray(0, 0, 0, nullptr);
unsigned int NzByteArray::npos(static_cast<unsigned int>(-1));
*/

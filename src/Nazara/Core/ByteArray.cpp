// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Math/Basic.hpp>
#include <algorithm>
#include <cstring>
#include <limits>
#include <Nazara/Core/Debug.hpp>

// Cet algorithme est inspiré de la documentation de Qt
inline unsigned int nzGetNewSize(unsigned int newSize)
{
	if (newSize < 20)
		return newSize+4;
	else
	{
		if (newSize < (1 << 12)-12)
			return NzGetNearestPowerOfTwo(newSize << 1)-12;
		else
			return newSize + (1 << 11);
	}
}

NzByteArray::NzByteArray() :
m_sharedArray(&emptyArray)
{
}

NzByteArray::NzByteArray(const nzUInt8* buffer, unsigned int length)
{
	if (length > 0)
	{
		m_sharedArray = new SharedArray;
		m_sharedArray->buffer = new nzUInt8[length];
		m_sharedArray->capacity = length;
		m_sharedArray->size = length;
		std::memcpy(m_sharedArray->buffer, buffer, length);
	}
	else
		m_sharedArray = &emptyArray;
}

NzByteArray::NzByteArray(const NzByteArray& buffer) :
m_sharedArray(buffer.m_sharedArray)
{
	if (m_sharedArray != &emptyArray)
		m_sharedArray->refCount++;
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

NzByteArray& NzByteArray::Append(nzUInt8 byte)
{
	return Insert(m_sharedArray->size, byte);
}

NzByteArray& NzByteArray::Append(const nzUInt8* buffer, unsigned int length)
{
	return Insert(m_sharedArray->size, buffer, length);
}

NzByteArray& NzByteArray::Append(const NzByteArray& array)
{
	return Insert(m_sharedArray->size, array);
}

void NzByteArray::Clear(bool keepBuffer)
{
	if (keepBuffer)
	{
		EnsureOwnership();
		m_sharedArray->size = 0;
	}
	else
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

NzByteArray& NzByteArray::Insert(int pos, nzUInt8 byte)
{
	return Insert(pos, &byte, 1);
}

NzByteArray& NzByteArray::Insert(int pos, const nzUInt8* buffer, unsigned int length)
{
	if (length == 0)
		return *this;

	if (pos < 0)
		pos = std::max(static_cast<int>(m_sharedArray->size + pos), 0);

	unsigned int start = std::min(static_cast<unsigned int>(pos), m_sharedArray->size);

	// Si le buffer est déjà suffisamment grand
	if (m_sharedArray->capacity >= m_sharedArray->size + length)
	{
		EnsureOwnership();

		std::memmove(&m_sharedArray->buffer[start+length], &m_sharedArray->buffer[start], m_sharedArray->size - start);
		std::memcpy(&m_sharedArray->buffer[start], buffer, length);

		m_sharedArray->size += length;
	}
	else
	{
		unsigned int newSize = m_sharedArray->size + length;
		nzUInt8* newBuffer = new nzUInt8[newSize];

		nzUInt8* ptr = newBuffer;

		if (start > 0)
		{
			std::memcpy(ptr, m_sharedArray->buffer, start*sizeof(nzUInt8));
			ptr += start;
		}

		std::memcpy(ptr, buffer, length*sizeof(nzUInt8));
		ptr += length;

		if (m_sharedArray->size > 0)
			std::memcpy(ptr, &m_sharedArray->buffer[start], m_sharedArray->size - start);

		ReleaseArray();
		m_sharedArray = new SharedArray;
		m_sharedArray->buffer = newBuffer;
		m_sharedArray->capacity = newSize;
		m_sharedArray->size = newSize;
	}

	return *this;

	return *this;
}

NzByteArray& NzByteArray::Insert(int pos, const NzByteArray& array)
{
	return Insert(pos, array.m_sharedArray->buffer, array.m_sharedArray->size);
}

bool NzByteArray::IsEmpty() const
{
	return m_sharedArray->size == 0;
}

NzByteArray& NzByteArray::Prepend(nzUInt8 byte)
{
	return Insert(0, byte);
}

NzByteArray& NzByteArray::Prepend(const nzUInt8* buffer, unsigned int length)
{
	return Insert(0, buffer, length);
}

NzByteArray& NzByteArray::Prepend(const NzByteArray& array)
{
	return Insert(0, array);
}

void NzByteArray::Reserve(unsigned int bufferSize)
{
	if (m_sharedArray->capacity >= bufferSize)
		return;

	nzUInt8* newBuffer = new nzUInt8[bufferSize];
	if (m_sharedArray->size > 0)
		std::memcpy(newBuffer, m_sharedArray->buffer, m_sharedArray->size);

	unsigned int size = m_sharedArray->size;

	ReleaseArray();
	m_sharedArray = new SharedArray;
	m_sharedArray->buffer = newBuffer;
	m_sharedArray->capacity = bufferSize;
	m_sharedArray->size = size;
}

NzByteArray& NzByteArray::Resize(int size)
{
	if (size == 0)
	{
		Clear(true);
		return *this;
	}

	if (size < 0)
		size = std::max(static_cast<int>(m_sharedArray->size + size), 0);

	unsigned int newSize = static_cast<unsigned int>(size);

	if (m_sharedArray->capacity >= newSize)
	{
		EnsureOwnership();

		// Nous avons déjà la place requise
		m_sharedArray->size = newSize;
	}
	else // On veut forcément agrandir la chaine
	{
		nzUInt8* newBuffer = new nzUInt8[newSize];
		if (m_sharedArray->size != 0)
			std::memcpy(newBuffer, m_sharedArray->buffer, newSize);

		ReleaseArray();
		m_sharedArray = new SharedArray;
		m_sharedArray->buffer = newBuffer;
		m_sharedArray->capacity = newSize;
		m_sharedArray->size = newSize;
	}

	return *this;
}

NzByteArray& NzByteArray::Resize(int size, nzUInt8 byte)
{
	if (size == 0)
	{
		Clear(true);
		return *this;
	}

	if (size < 0)
		size = std::max(static_cast<int>(m_sharedArray->size + size), 0);

	unsigned int newSize = static_cast<unsigned int>(size);

	if (m_sharedArray->capacity >= newSize)
	{
		EnsureOwnership();

		// Nous avons déjà la place requise, contentons-nous de remplir le buffer
		if (newSize > m_sharedArray->size)
			std::memset(&m_sharedArray->buffer[m_sharedArray->size], byte, newSize-m_sharedArray->size);

		m_sharedArray->size = newSize;
	}
	else // On veut forcément agrandir la chaine
	{
		nzUInt8* newBuffer = new nzUInt8[newSize];
		if (m_sharedArray->size != 0)
			std::memcpy(newBuffer, m_sharedArray->buffer, newSize);

		std::memset(&newBuffer[m_sharedArray->size], byte, newSize-m_sharedArray->size);

		ReleaseArray();
		m_sharedArray = new SharedArray;
		m_sharedArray->buffer = newBuffer;
		m_sharedArray->capacity = newSize;
		m_sharedArray->size = newSize;
	}

	return *this;
}

NzByteArray NzByteArray::Resized(int size) const
{
	if (size < 0)
		size = m_sharedArray->size + size;

	if (size <= 0)
		return NzByteArray();

	unsigned int newSize = static_cast<unsigned int>(size);
	if (newSize == m_sharedArray->size)
		return *this;

	nzUInt8* buffer = new nzUInt8[newSize];
	std::memcpy(buffer, m_sharedArray->buffer, (newSize > m_sharedArray->size) ? m_sharedArray->size : newSize);

	return NzByteArray(new SharedArray(1, newSize, newSize, buffer));
}

NzByteArray NzByteArray::Resized(int size, nzUInt8 byte) const
{
	if (size < 0)
		size = m_sharedArray->size + size;

	if (size <= 0)
		return NzByteArray();

	unsigned int newSize = static_cast<unsigned int>(size);
	if (newSize == m_sharedArray->size)
		return *this;

	nzUInt8* buffer = new nzUInt8[newSize];
	if (newSize > m_sharedArray->size)
	{
		std::memcpy(buffer, m_sharedArray->buffer, m_sharedArray->size);
		std::memset(&buffer[m_sharedArray->size], byte, newSize - m_sharedArray->size);
	}
	else
		std::memcpy(buffer, m_sharedArray->buffer, newSize);

	return NzByteArray(new SharedArray(1, newSize, newSize, buffer));
}

NzByteArray NzByteArray::SubArray(int startPos, int endPos) const
{
	if (startPos < 0)
		startPos = std::max(m_sharedArray->size+startPos, 0U);

	unsigned int start = static_cast<unsigned int>(startPos);

	if (endPos < 0)
	{
		endPos = m_sharedArray->size + endPos;
		if (endPos < 0)
			return NzByteArray();
	}

	unsigned int minEnd = std::min(static_cast<unsigned int>(endPos), m_sharedArray->size-1);

	if (start > minEnd || start >= m_sharedArray->size)
		return NzByteArray();

	unsigned int size = minEnd - start + 1;
	nzUInt8* buffer = new nzUInt8[size];
	std::memcpy(buffer, &m_sharedArray->buffer[start], size);

	return NzByteArray(new SharedArray(1, size, size, buffer));
}

void NzByteArray::Swap(NzByteArray& array)
{
	std::swap(m_sharedArray, array.m_sharedArray);
}

nzUInt8* NzByteArray::begin()
{
	return m_sharedArray->buffer;
}

const nzUInt8* NzByteArray::begin() const
{
	return m_sharedArray->buffer;
}

nzUInt8* NzByteArray::end()
{
	return &m_sharedArray->buffer[m_sharedArray->size];
}

const nzUInt8* NzByteArray::end() const
{
	return &m_sharedArray->buffer[m_sharedArray->size];
}

void NzByteArray::push_front(nzUInt8 byte)
{
	Prepend(byte);
}

void NzByteArray::push_back(nzUInt8 byte)
{
	Append(byte);
}

nzUInt8& NzByteArray::operator[](unsigned int pos)
{
	EnsureOwnership();

	if (pos >= m_sharedArray->size)
		Resize(pos+1);

	return m_sharedArray->buffer[pos];
}

nzUInt8 NzByteArray::operator[](unsigned int pos) const
{
	#if NAZARA_CORE_SAFE
	if (pos >= m_sharedArray->size)
	{
		NazaraError("Index out of range (" + NzString::Number(pos) + " >= " + NzString::Number(m_sharedArray->size) + ')');
		return 0;
	}
	#endif

	return m_sharedArray->buffer[pos];
}

NzByteArray& NzByteArray::operator=(const NzByteArray& array)
{
	ReleaseArray();

	m_sharedArray = array.m_sharedArray;
	if (m_sharedArray != &emptyArray)
		m_sharedArray->refCount++;

	return *this;
}

NzByteArray& NzByteArray::operator=(NzByteArray&& array) noexcept
{
	std::swap(m_sharedArray, array.m_sharedArray);

	return *this;
}

NzByteArray NzByteArray::operator+(nzUInt8 byte) const
{
	if (m_sharedArray->size == 0)
		return NzByteArray(&byte, 1);

	unsigned int totalSize = m_sharedArray->size + 1;
	nzUInt8* buffer = new nzUInt8[totalSize];
	std::memcpy(buffer, m_sharedArray->buffer, m_sharedArray->size);
	buffer[m_sharedArray->size] = byte;

	return NzByteArray(new SharedArray(1, totalSize, totalSize, buffer));
}

NzByteArray NzByteArray::operator+(const NzByteArray& array) const
{
	if (array.m_sharedArray->size == 0)
		return *this;

	if (m_sharedArray->size == 0)
		return array;

	unsigned int totalSize = m_sharedArray->size + array.m_sharedArray->size;
	nzUInt8* buffer = new nzUInt8[totalSize];
	std::memcpy(buffer, m_sharedArray->buffer, m_sharedArray->size);
	std::memcpy(&buffer[m_sharedArray->size], array.m_sharedArray->buffer, array.m_sharedArray->size);

	return NzByteArray(new SharedArray(1, totalSize, totalSize, buffer));
}

NzByteArray& NzByteArray::operator+=(nzUInt8 byte)
{
	return Append(byte);
}

NzByteArray& NzByteArray::operator+=(const NzByteArray& array)
{
	return Append(array);
}

int NzByteArray::Compare(const NzByteArray& first, const NzByteArray& second)
{
	return std::memcmp(first.m_sharedArray->buffer, second.m_sharedArray->buffer, std::min(first.m_sharedArray->size, second.m_sharedArray->size));
}

void NzByteArray::EnsureOwnership()
{
	if (m_sharedArray == &emptyArray)
		return;

	if (m_sharedArray->refCount > 1)
	{
		m_sharedArray->refCount--;

		nzUInt8* buffer = new nzUInt8[m_sharedArray->capacity];
		std::memcpy(buffer, m_sharedArray->buffer, m_sharedArray->size);

		m_sharedArray = new SharedArray(1, m_sharedArray->capacity, m_sharedArray->size, buffer);
	}
}

bool NzByteArray::FillHash(NzAbstractHash* hazh) const
{
	hazh->Append(m_sharedArray->buffer, m_sharedArray->size);

	return true;
}

void NzByteArray::ReleaseArray()
{
	if (m_sharedArray == &emptyArray)
		return;

	if (--m_sharedArray->refCount == 0)
	{
		delete[] m_sharedArray->buffer;
		delete m_sharedArray;
	}

	m_sharedArray = &emptyArray;
}

NzByteArray::SharedArray NzByteArray::emptyArray(0, 0, 0, nullptr);
unsigned int NzByteArray::npos(std::numeric_limits<unsigned int>::max());

namespace std
{
	void swap(NzByteArray& lhs, NzByteArray& rhs)
	{
		lhs.Swap(rhs);
	}
}

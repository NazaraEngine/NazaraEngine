// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ByteArray.hpp>

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Debug.hpp>

 NzByteArray::iterator NzByteArray::begin() noexcept
{
	return m_array.begin();
}

NzByteArray::const_iterator NzByteArray::begin() const noexcept
{
	return m_array.begin();
}

 NzByteArray::iterator NzByteArray::end() noexcept
{
	return m_array.end();
}

NzByteArray::const_iterator NzByteArray::end() const noexcept
{
	return m_array.end();
}

NzByteArray::reverse_iterator NzByteArray::rbegin() noexcept
{
	return m_array.rbegin();
}

NzByteArray::const_reverse_iterator NzByteArray::rbegin() const noexcept
{
	return m_array.rbegin();
}

NzByteArray::reverse_iterator NzByteArray::rend() noexcept
{
	return m_array.rend();
}

NzByteArray::const_iterator NzByteArray::cbegin() const noexcept
{
	return m_array.cbegin();
}

NzByteArray::const_iterator NzByteArray::cend() const noexcept
{
	return m_array.cend();
}

NzByteArray::const_reverse_iterator NzByteArray::crbegin() const noexcept
{
	return m_array.crbegin();
}

NzByteArray::const_reverse_iterator NzByteArray::crend() const noexcept
{
	return m_array.crend();
}

NzByteArray::NzByteArray() :
m_array()
{
}

NzByteArray::NzByteArray(size_type n) :
m_array()
{
	m_array.reserve(n);
}

NzByteArray::NzByteArray(const void* buffer, size_type n) :
m_array(static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n)
{
}

NzByteArray::NzByteArray(size_type n, const value_type value) :
m_array(n, value)
{
}

NzByteArray::NzByteArray(const NzByteArray& other) :
m_array(other.m_array)
{
}

NzByteArray::NzByteArray(NzByteArray&& other) :
m_array(std::move(other.m_array))
{
}

NzByteArray::~NzByteArray()
{
}

NzByteArray::iterator NzByteArray::Append(const void* buffer, size_type n)
{
	return Insert(end(), static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n);
}

NzByteArray::iterator NzByteArray::Append(const NzByteArray& other)
{
	return Insert(end(), other.begin(), other.end());
}

void NzByteArray::Assign(size_type n, const value_type value)
{
	m_array.assign(n, value);
}

NzByteArray::reference NzByteArray::Back()
{
	return m_array.back();
}

NzByteArray::const_reference NzByteArray::Back() const
{
	return m_array.back();
}

void NzByteArray::Clear(bool keepBuffer)
{
	m_array.clear();

	if (!keepBuffer)
		ShrinkToFit();
}

NzByteArray::pointer NzByteArray::data()
{
	return GetBuffer();
}

NzByteArray::const_pointer NzByteArray::data() const
{
	return GetConstBuffer();
}

bool NzByteArray::empty() const noexcept
{
	return IsEmpty();
}

NzByteArray::iterator NzByteArray::Erase(const_iterator pos)
{
	return m_array.erase(pos);
}

NzByteArray::iterator NzByteArray::Erase(const_iterator first, const_iterator last)
{
	return m_array.erase(first, last);
}

NzByteArray::reference NzByteArray::Front()
{
	return m_array.front();
}

NzByteArray::const_reference NzByteArray::Front() const
{
	return m_array.front();
}

NzByteArray::iterator NzByteArray::Insert(const_iterator pos, const void* buffer, size_type n)
{
	return Insert(pos, static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n);
}

NzByteArray::iterator NzByteArray::Insert(const_iterator pos, const NzByteArray& other)
{
	return Insert(pos, other.begin(), other.end());
}

NzByteArray::iterator NzByteArray::Insert(const_iterator pos, size_type n, const value_type byte)
{
	return Insert(pos, n, byte);
}

bool NzByteArray::IsEmpty() const noexcept
{
	return m_array.empty();
}

NzByteArray::allocator_type NzByteArray::GetAllocator() const
{
	return m_array.get_allocator();
}

NzByteArray::pointer NzByteArray::GetBuffer()
{
	return m_array.data();
}

NzByteArray::size_type NzByteArray::GetCapacity() const noexcept
{
	return m_array.capacity();
}

NzByteArray::const_pointer NzByteArray::GetConstBuffer() const
{
	return m_array.data();
}

NzByteArray::size_type NzByteArray::GetSize() const noexcept
{
	return m_array.size();
}

NzByteArray::size_type NzByteArray::MaxSize() const noexcept
{
	return m_array.max_size();
}

NzByteArray::reference NzByteArray::operator[](size_type pos)
{
	#if NAZARA_CORE_SAFE
	if (pos >= GetSize())
		NazaraError("Index out of range (" + NzString::Number(pos) + " >= " + NzString::Number(GetSize()) + ')');
	#endif

	return m_array[pos];
}

NzByteArray::const_reference NzByteArray::operator[](size_type pos) const
{
	#if NAZARA_CORE_SAFE
	if (pos >= GetSize())
		NazaraError("Index out of range (" + NzString::Number(pos) + " >= " + NzString::Number(GetSize()) + ')');
	#endif

	return m_array[pos];
}

NzByteArray& NzByteArray::operator=(const NzByteArray& other)
{
	m_array = other.m_array;

	return *this;
}

NzByteArray& NzByteArray::operator=(NzByteArray&& other)
{
	m_array = std::move(other.m_array);

	return *this;
}

NzByteArray NzByteArray::operator+(const NzByteArray& other) const
{
	NzByteArray tmp(*this);
	tmp += other;

	return tmp;
}

NzByteArray& NzByteArray::operator+=(const NzByteArray& other)
{
	Append(other);

	return *this;
}

bool NzByteArray::operator==(const NzByteArray& rhs) const
{
	return m_array == rhs.m_array;
}

bool NzByteArray::operator!=(const NzByteArray& rhs) const
{
	return !operator==(rhs);
}

bool NzByteArray::operator<(const NzByteArray& rhs) const
{
	return m_array < rhs.m_array;
}

bool NzByteArray::operator<=(const NzByteArray& rhs) const
{
	return m_array <= rhs.m_array;
}

bool NzByteArray::operator>(const NzByteArray& rhs) const
{
	return m_array > rhs.m_array;
}

bool NzByteArray::operator>=(const NzByteArray& rhs) const
{
	return m_array >= rhs.m_array;
}

void NzByteArray::PopBack()
{
	Erase(end() - 1);
}

void NzByteArray::PopFront()
{
	Erase(begin());
}

NzByteArray::iterator NzByteArray::Prepend(const void* buffer, size_type n)
{
	return Insert(begin(), buffer, n);
}

NzByteArray::iterator NzByteArray::Prepend(const NzByteArray& other)
{
	return Insert(begin(), other);
}

void NzByteArray::PushBack(const value_type byte)
{
	m_array.push_back(byte);
}

void NzByteArray::PushFront(const value_type byte)
{
	m_array.insert(begin(), 1, byte);
}

void NzByteArray::Reserve(size_type bufferSize)
{
	m_array.reserve(bufferSize);
}

void NzByteArray::Resize(size_type newSize)
{
	m_array.resize(newSize);
}

void NzByteArray::Resize(size_type newSize, const value_type byte)
{
	m_array.resize(newSize, byte);
}

NzByteArray::size_type NzByteArray::size() const noexcept
{
	return GetSize();
}

void NzByteArray::ShrinkToFit()
{
	m_array.shrink_to_fit();
}

NzByteArray NzByteArray::SubArray(const_iterator startPos, const_iterator endPos) const
{
	return NzByteArray(startPos, endPos);
}

void NzByteArray::Swap(NzByteArray& other)
{
	m_array.swap(other.m_array);
}

NzString NzByteArray::ToString() const
{
	NzStringStream ss;

	for (const auto& it : m_array)
		ss << it;

	return ss;
}

bool NzByteArray::FillHash(NzAbstractHash* hash) const
{
	hash->Append(GetConstBuffer(), GetSize());

	return true;
}

std::ostream& operator<<(std::ostream& out, const NzByteArray& byteArray)
{
	out << byteArray.ToString();
	return out;
}

namespace std
{
	void swap(NzByteArray& lhs, NzByteArray& rhs)
	{
		lhs.Swap(rhs);
	}
}

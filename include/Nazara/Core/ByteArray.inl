// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

inline NzByteArray::NzByteArray(size_type n) :
m_array()
{
	m_array.reserve(n);
}

inline NzByteArray::NzByteArray(const void* buffer, size_type n) :
m_array(static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n)
{
}

inline NzByteArray::NzByteArray(size_type n, const value_type value) :
m_array(n, value)
{
}

template <class InputIterator>
NzByteArray::NzByteArray(InputIterator first, InputIterator last) :
m_array(first, last)
{
}

inline NzByteArray::iterator NzByteArray::Append(const void* buffer, size_type n)
{
	return Insert(end(), static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n);
}

inline NzByteArray::iterator NzByteArray::Append(const NzByteArray& other)
{
	return Insert(end(), other.begin(), other.end());
}

inline void NzByteArray::Assign(size_type n, value_type value)
{
	m_array.assign(n, value);
}

template <class InputIterator>
void NzByteArray::Assign(InputIterator first, InputIterator last)
{
	m_array.assign(first, last);
}

inline NzByteArray::reference NzByteArray::Back()
{
	return m_array.back();
}

inline NzByteArray::const_reference NzByteArray::Back() const
{
	return m_array.back();
}

inline NzByteArray::iterator NzByteArray::Erase(const_iterator pos)
{
	return m_array.erase(pos);
}

inline NzByteArray::iterator NzByteArray::Erase(const_iterator first, const_iterator last)
{
	return m_array.erase(first, last);
}

inline NzByteArray::allocator_type NzByteArray::GetAllocator() const
{
	return m_array.get_allocator();
}

inline NzByteArray::pointer NzByteArray::GetBuffer()
{
	return m_array.data();
}

inline NzByteArray::size_type NzByteArray::GetCapacity() const noexcept
{
	return m_array.capacity();
}

inline NzByteArray::const_pointer NzByteArray::GetConstBuffer() const
{
	return m_array.data();
}

inline NzByteArray::size_type NzByteArray::GetMaxSize() const noexcept
{
	return m_array.max_size();
}

inline NzByteArray::size_type NzByteArray::GetSize() const noexcept
{
	return m_array.size();
}

inline NzByteArray NzByteArray::GetSubArray(const_iterator startPos, const_iterator endPos) const
{
	return NzByteArray(startPos, endPos);
}

inline NzByteArray::iterator NzByteArray::Insert(const_iterator pos, const void* buffer, size_type n)
{
	return Insert(pos, static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n);
}

inline NzByteArray::iterator NzByteArray::Insert(const_iterator pos, const NzByteArray& other)
{
	return Insert(pos, other.begin(), other.end());
}

inline NzByteArray::iterator NzByteArray::Insert(const_iterator pos, size_type n, const value_type byte)
{
	return Insert(pos, n, byte);
}

template <class InputIterator>
NzByteArray::iterator NzByteArray::Insert(const_iterator pos, InputIterator first, InputIterator last)
{
	return m_array.insert(pos, first, last);
}

inline bool NzByteArray::IsEmpty() const noexcept
{
	return m_array.empty();
}

inline void NzByteArray::PopBack()
{
	Erase(end() - 1);
}

inline void NzByteArray::PopFront()
{
	Erase(begin());
}

inline NzByteArray::iterator NzByteArray::Prepend(const void* buffer, size_type n)
{
	return Insert(begin(), buffer, n);
}

inline NzByteArray::iterator NzByteArray::Prepend(const NzByteArray& other)
{
	return Insert(begin(), other);
}

inline void NzByteArray::PushBack(const value_type byte)
{
	m_array.push_back(byte);
}

inline void NzByteArray::PushFront(const value_type byte)
{
	m_array.insert(begin(), 1, byte);
}

inline void NzByteArray::Reserve(size_type bufferSize)
{
	m_array.reserve(bufferSize);
}

inline void NzByteArray::Resize(size_type newSize)
{
	m_array.resize(newSize);
}

inline void NzByteArray::Resize(size_type newSize, const value_type byte)
{
	m_array.resize(newSize, byte);
}

inline void NzByteArray::ShrinkToFit()
{
	m_array.shrink_to_fit();
}

inline void NzByteArray::Swap(NzByteArray& other)
{
	m_array.swap(other.m_array);
}

inline NzString NzByteArray::ToString() const
{
	return NzString(reinterpret_cast<const char*>(GetConstBuffer()), GetSize());
}

inline NzByteArray::iterator NzByteArray::begin() noexcept
{
	return m_array.begin();
}

inline NzByteArray::const_iterator NzByteArray::begin() const noexcept
{
	return m_array.begin();
}

inline NzByteArray::const_iterator NzByteArray::cbegin() const noexcept
{
	return m_array.cbegin();
}

inline NzByteArray::const_iterator NzByteArray::cend() const noexcept
{
	return m_array.cend();
}

inline NzByteArray::const_reverse_iterator NzByteArray::crbegin() const noexcept
{
	return m_array.crbegin();
}

inline NzByteArray::const_reverse_iterator NzByteArray::crend() const noexcept
{
	return m_array.crend();
}

inline bool NzByteArray::empty() const noexcept
{
	return m_array.empty();
}

inline NzByteArray::iterator NzByteArray::end() noexcept
{
	return m_array.end();
}

inline NzByteArray::const_iterator NzByteArray::end() const noexcept
{
	return m_array.end();
}

inline NzByteArray::reverse_iterator NzByteArray::rbegin() noexcept
{
	return m_array.rbegin();
}

inline NzByteArray::const_reverse_iterator NzByteArray::rbegin() const noexcept
{
	return m_array.rbegin();
}

inline NzByteArray::reverse_iterator NzByteArray::rend() noexcept
{
	return m_array.rend();
}

inline NzByteArray::size_type NzByteArray::size() const noexcept
{
	return GetSize();
}

inline NzByteArray::reference NzByteArray::operator[](size_type pos)
{
	NazaraAssert(pos < GetSize(), "Index out of range");

	return m_array[pos];
}

inline NzByteArray::const_reference NzByteArray::operator[](size_type pos) const
{
	NazaraAssert(pos < GetSize(), "Index out of range");

	return m_array[pos];
}

inline NzByteArray NzByteArray::operator+(const NzByteArray& other) const
{
	NzByteArray tmp(*this);
	tmp += other;

	return tmp;
}

inline NzByteArray& NzByteArray::operator+=(const NzByteArray& other)
{
	Append(other);

	return *this;
}

inline bool NzByteArray::operator==(const NzByteArray& rhs) const
{
	return m_array == rhs.m_array;
}

inline bool NzByteArray::operator!=(const NzByteArray& rhs) const
{
	return !operator==(rhs);
}

inline bool NzByteArray::operator<(const NzByteArray& rhs) const
{
	return m_array < rhs.m_array;
}

inline bool NzByteArray::operator<=(const NzByteArray& rhs) const
{
	return m_array <= rhs.m_array;
}

inline bool NzByteArray::operator>(const NzByteArray& rhs) const
{
	return m_array > rhs.m_array;
}

inline bool NzByteArray::operator>=(const NzByteArray& rhs) const
{
	return m_array >= rhs.m_array;
}

namespace std
{
	inline void swap(NzByteArray& lhs, NzByteArray& rhs)
	{
		lhs.Swap(rhs);
	}
}

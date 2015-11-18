// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>

namespace Nz
{
	inline ByteArray::ByteArray(size_type n) :
	m_array()
	{
		m_array.reserve(n);
	}

	inline ByteArray::ByteArray(const void* buffer, size_type n) :
	m_array(static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n)
	{
	}

	inline ByteArray::ByteArray(size_type n, const value_type value) :
	m_array(n, value)
	{
	}

	template <class InputIterator>
	ByteArray::ByteArray(InputIterator first, InputIterator last) :
	m_array(first, last)
	{
	}

	inline ByteArray::iterator ByteArray::Append(const void* buffer, size_type n)
	{
		return Insert(end(), static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n);
	}

	inline ByteArray::iterator ByteArray::Append(const ByteArray& other)
	{
		return Insert(end(), other.begin(), other.end());
	}

	inline void ByteArray::Assign(size_type n, value_type value)
	{
		m_array.assign(n, value);
	}

	template <class InputIterator>
	void ByteArray::Assign(InputIterator first, InputIterator last)
	{
		m_array.assign(first, last);
	}

	inline ByteArray::reference ByteArray::Back()
	{
		return m_array.back();
	}

	inline ByteArray::const_reference ByteArray::Back() const
	{
		return m_array.back();
	}

	inline ByteArray::iterator ByteArray::Erase(const_iterator pos)
	{
		return m_array.erase(pos);
	}

	inline ByteArray::iterator ByteArray::Erase(const_iterator first, const_iterator last)
	{
		return m_array.erase(first, last);
	}

	inline ByteArray::reference ByteArray::Front()
	{
		return m_array.front();
	}

	inline ByteArray::const_reference ByteArray::Front() const
	{
		return m_array.front();
	}

	inline ByteArray::allocator_type ByteArray::GetAllocator() const
	{
		return m_array.get_allocator();
	}

	inline ByteArray::pointer ByteArray::GetBuffer()
	{
		return m_array.data();
	}

	inline ByteArray::size_type ByteArray::GetCapacity() const noexcept
	{
		return m_array.capacity();
	}

	inline ByteArray::const_pointer ByteArray::GetConstBuffer() const
	{
		return m_array.data();
	}

	inline ByteArray::size_type ByteArray::GetMaxSize() const noexcept
	{
		return m_array.max_size();
	}

	inline ByteArray::size_type ByteArray::GetSize() const noexcept
	{
		return m_array.size();
	}

	inline ByteArray ByteArray::GetSubArray(const_iterator startPos, const_iterator endPos) const
	{
		return ByteArray(startPos, endPos);
	}

	inline ByteArray::iterator ByteArray::Insert(const_iterator pos, const void* buffer, size_type n)
	{
		return m_array.insert(pos, static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n);
	}

	inline ByteArray::iterator ByteArray::Insert(const_iterator pos, const ByteArray& other)
	{
		return m_array.insert(pos, other.begin(), other.end());
	}

	inline ByteArray::iterator ByteArray::Insert(const_iterator pos, size_type n, value_type byte)
	{
		return m_array.insert(pos, n, byte);
	}

	template <class InputIterator>
	ByteArray::iterator ByteArray::Insert(const_iterator pos, InputIterator first, InputIterator last)
	{
		return m_array.insert(pos, first, last);
	}

	inline bool ByteArray::IsEmpty() const noexcept
	{
		return m_array.empty();
	}

	inline void ByteArray::PopBack()
	{
		Erase(end() - 1);
	}

	inline void ByteArray::PopFront()
	{
		Erase(begin());
	}

	inline ByteArray::iterator ByteArray::Prepend(const void* buffer, size_type n)
	{
		return Insert(begin(), buffer, n);
	}

	inline ByteArray::iterator ByteArray::Prepend(const ByteArray& other)
	{
		return Insert(begin(), other);
	}

	inline void ByteArray::PushBack(const value_type byte)
	{
		m_array.push_back(byte);
	}

	inline void ByteArray::PushFront(const value_type byte)
	{
		m_array.insert(begin(), 1, byte);
	}

	inline void ByteArray::Reserve(size_type bufferSize)
	{
		m_array.reserve(bufferSize);
	}

	inline void ByteArray::Resize(size_type newSize)
	{
		m_array.resize(newSize);
	}

	inline void ByteArray::Resize(size_type newSize, const value_type byte)
	{
		m_array.resize(newSize, byte);
	}

	inline void ByteArray::ShrinkToFit()
	{
		m_array.shrink_to_fit();
	}

	inline void ByteArray::Swap(ByteArray& other)
	{
		m_array.swap(other.m_array);
	}

	inline String ByteArray::ToHex() const
	{
		unsigned int length = m_array.size() * 2;

		String hexOutput(length, '\0');
		for (unsigned int i = 0; i < m_array.size(); ++i)
			std::sprintf(&hexOutput[i * 2], "%02x", m_array[i]);

		return hexOutput;
	}

	inline String ByteArray::ToString() const
	{
		return String(reinterpret_cast<const char*>(GetConstBuffer()), GetSize());
	}

	inline ByteArray::iterator ByteArray::begin() noexcept
	{
		return m_array.begin();
	}

	inline ByteArray::const_iterator ByteArray::begin() const noexcept
	{
		return m_array.begin();
	}

	inline ByteArray::const_iterator ByteArray::cbegin() const noexcept
	{
		return m_array.cbegin();
	}

	inline ByteArray::const_iterator ByteArray::cend() const noexcept
	{
		return m_array.cend();
	}

	inline ByteArray::const_reverse_iterator ByteArray::crbegin() const noexcept
	{
		return m_array.crbegin();
	}

	inline ByteArray::const_reverse_iterator ByteArray::crend() const noexcept
	{
		return m_array.crend();
	}

	inline bool ByteArray::empty() const noexcept
	{
		return m_array.empty();
	}

	inline ByteArray::iterator ByteArray::end() noexcept
	{
		return m_array.end();
	}

	inline ByteArray::const_iterator ByteArray::end() const noexcept
	{
		return m_array.end();
	}

	inline ByteArray::reverse_iterator ByteArray::rbegin() noexcept
	{
		return m_array.rbegin();
	}

	inline ByteArray::const_reverse_iterator ByteArray::rbegin() const noexcept
	{
		return m_array.rbegin();
	}

	inline ByteArray::reverse_iterator ByteArray::rend() noexcept
	{
		return m_array.rend();
	}

	inline ByteArray::size_type ByteArray::size() const noexcept
	{
		return GetSize();
	}

	inline ByteArray::reference ByteArray::operator[](size_type pos)
	{
		NazaraAssert(pos < GetSize(), "Index out of range");

		return m_array[pos];
	}

	inline ByteArray::const_reference ByteArray::operator[](size_type pos) const
	{
		NazaraAssert(pos < GetSize(), "Index out of range");

		return m_array[pos];
	}

	inline ByteArray ByteArray::operator+(const ByteArray& other) const
	{
		ByteArray tmp(*this);
		tmp += other;

		return tmp;
	}

	inline ByteArray& ByteArray::operator+=(const ByteArray& other)
	{
		Append(other);

		return *this;
	}

	inline bool ByteArray::operator==(const ByteArray& rhs) const
	{
		return m_array == rhs.m_array;
	}

	inline bool ByteArray::operator!=(const ByteArray& rhs) const
	{
		return !operator==(rhs);
	}

	inline bool ByteArray::operator<(const ByteArray& rhs) const
	{
		return m_array < rhs.m_array;
	}

	inline bool ByteArray::operator<=(const ByteArray& rhs) const
	{
		return m_array <= rhs.m_array;
	}

	inline bool ByteArray::operator>(const ByteArray& rhs) const
	{
		return m_array > rhs.m_array;
	}

	inline bool ByteArray::operator>=(const ByteArray& rhs) const
	{
		return m_array >= rhs.m_array;
	}

	inline bool HashAppend(AbstractHash* hash, const ByteArray& byteArray)
	{
		hash->Append(byteArray.GetConstBuffer(), byteArray.GetSize());
		return true;
	}
}

namespace std
{
	inline void swap(Nz::ByteArray& lhs, Nz::ByteArray& rhs)
	{
		lhs.Swap(rhs);
	}
}

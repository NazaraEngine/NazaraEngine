// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <cstdio>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a ByteArray object with a reserved size
	*
	* \param n Space reserved
	*/

	inline ByteArray::ByteArray(size_type n) :
	m_array()
	{
		m_array.reserve(n);
	}

	/*!
	* \brief Constructs a ByteArray object with a raw memory and a size
	*
	* \param buffer Pointer to raw memory
	* \param n Size that can be accessed
	*
	* \remark If preallocated space of buffer is less than the size, the behaviour is undefined
	*/

	inline ByteArray::ByteArray(const void* buffer, size_type n) :
	m_array(static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n)
	{
	}

	/*!
	* \brief Constructs a ByteArray object with n times the same value
	*
	* \param n Number of repetitions
	* \param value Value to repeat
	*/

	inline ByteArray::ByteArray(size_type n, const value_type value) :
	m_array(n, value)
	{
	}

	/*!
	* \brief Constructs a ByteArray object from two iterators
	*
	* \param first First iterator
	* \param last Second iterator
	*/

	template <class InputIterator>
	ByteArray::ByteArray(InputIterator first, InputIterator last) :
	m_array(first, last)
	{
	}

	/*!
	* \brief Appends the content of raw memory
	*
	* \param buffer Constant pointer to raw memory
	* \param n Size that can be read
	*
	* \remark If preallocated space of buffer is less than the size, the behaviour is undefined
	*
	* \see Insert
	*/

	inline ByteArray::iterator ByteArray::Append(const void* buffer, size_type n)
	{
		return Insert(end(), static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n);
	}

	/*!
	* \brief Appends another array of bytes
	*
	* \param other ByteArray to add
	*
	* \see Insert
	*/

	inline ByteArray::iterator ByteArray::Append(const ByteArray& other)
	{
		return Insert(end(), other.begin(), other.end());
	}

	/*!
	* \brief Assigns this with n times the same value
	*
	* \param n Number of repetitions
	* \param value Value to repeat
	*/

	inline void ByteArray::Assign(size_type n, value_type value)
	{
		m_array.assign(n, value);
	}

	/*!
	* \brief Assigns this with the content between two iterators
	*
	* \param first First iterator
	* \param last Second iterator
	*/

	template <class InputIterator>
	void ByteArray::Assign(InputIterator first, InputIterator last)
	{
		m_array.assign(first, last);
	}

	/*!
	* \brief Gets last element
	* \return A reference to last element
	*
	* \remark If ByteArray is empty, behaviour is undefined
	*/

	inline ByteArray::reference ByteArray::Back()
	{
		return m_array.back();
	}

	/*!
	* \brief Gets last element
	* \return A constant reference to last element
	*
	* \remark If ByteArray is empty, behaviour is undefined
	*/

	inline ByteArray::const_reference ByteArray::Back() const
	{
		return m_array.back();
	}

	/*!
	* \brief Clears the content of the string
	*
	* \param keepBuffer Should the buffer be kept
	*/

	inline void ByteArray::Clear(bool keepBuffer)
	{
		m_array.clear();
		if (!keepBuffer)
			m_array.shrink_to_fit();
	}

	/*!
	* \brief Erases an element from the byte array
	*
	* \param pos Iterator to the element
	*/

	inline ByteArray::iterator ByteArray::Erase(const_iterator pos)
	{
		return m_array.erase(pos);
	}

	/*!
	* \brief Erases the elements between the two pointers from the byte array
	*
	* \param first First iterator
	* \param last Second iterator
	*/

	inline ByteArray::iterator ByteArray::Erase(const_iterator first, const_iterator last)
	{
		return m_array.erase(first, last);
	}

	/*!
	* \brief Gets first element
	* \return A reference to first element
	*
	* \remark If ByteArray is empty, behaviour is undefined
	*/

	inline ByteArray::reference ByteArray::Front()
	{
		return m_array.front();
	}

	/*!
	* \brief Gets first element
	* \return A constant reference to first element
	*
	* \remark If ByteArray is empty, behaviour is undefined
	*/

	inline ByteArray::const_reference ByteArray::Front() const
	{
		return m_array.front();
	}

	/*!
	* \brief Gets the internal allocator of the byte array
	* \return Allocator
	*/

	inline ByteArray::allocator_type ByteArray::GetAllocator() const
	{
		return m_array.get_allocator();
	}

	/*!
	* \brief Gets the raw buffer
	* \return Raw buffer
	*/

	inline ByteArray::pointer ByteArray::GetBuffer()
	{
		return m_array.data();
	}

	/*!
	* \brief Gets the capacity of the byte array
	* \return Capacity of the byte array
	*/

	inline ByteArray::size_type ByteArray::GetCapacity() const noexcept
	{
		return m_array.capacity();
	}

	/*!
	* \brief Gets the raw buffer
	* \return Raw buffer
	*/

	inline ByteArray::const_pointer ByteArray::GetConstBuffer() const
	{
		return m_array.data();
	}

	/*!
	* \brief Gets the maximal size supported by the byte array
	* \return Biggest size handled
	*/

	inline ByteArray::size_type ByteArray::GetMaxSize() const noexcept
	{
		return m_array.max_size();
	}

	/*!
	* \brief Gets the size of the byte array
	* \return Size of the byte array
	*/

	inline ByteArray::size_type ByteArray::GetSize() const noexcept
	{
		return m_array.size();
	}

	/*!
	* \brief Returns a sub byte array of the byte array
	* \return Sub byte array
	*
	* \param startPos First iterator
	* \param endPos Second iterator
	*/

	inline ByteArray ByteArray::GetSubArray(const_iterator startPos, const_iterator endPos) const
	{
		return ByteArray(startPos, endPos);
	}

	/*!
	* \brief Inserts the content of raw memory at the iterator position
	*
	* \param pos Iterator to the position
	* \param buffer Constant pointer to raw memory
	* \param n Size that can be read
	*
	* \remark If preallocated space of ptr is less than the size, the behaviour is undefined
	*/

	inline ByteArray::iterator ByteArray::Insert(const_iterator pos, const void* buffer, size_type n)
	{
		return m_array.insert(pos, static_cast<const_pointer>(buffer), static_cast<const_pointer>(buffer) + n);
	}

	/*!
	* \brief Inserts the content of another byte array at the iterator position
	*
	* \param pos Iterator to the position
	* \param other Other byte array
	*/

	inline ByteArray::iterator ByteArray::Insert(const_iterator pos, const ByteArray& other)
	{
		return m_array.insert(pos, other.begin(), other.end());
	}

	/*!
	* \brief Inserts n times the same byte at the iterator position
	*
	* \param pos Iterator to the position
	* \param n Number of repetitions
	* \param byte Value to repeat
	*/

	inline ByteArray::iterator ByteArray::Insert(const_iterator pos, size_type n, value_type byte)
	{
		return m_array.insert(pos, n, byte);
	}

	/*!
	* \brief Inserts the content from two iterators at the iterator position
	*
	* \param pos Iterator to the position
	* \param first First iterator
	* \param last Second iterator
	*/

	template <class InputIterator>
	ByteArray::iterator ByteArray::Insert(const_iterator pos, InputIterator first, InputIterator last)
	{
		return m_array.insert(pos, first, last);
	}

	/*!
	* \brief Checks whether the byte array is empty
	* \return true if byte array is empty
	*/

	inline bool ByteArray::IsEmpty() const noexcept
	{
		return m_array.empty();
	}

	/*!
	* \brief Erases the last element
	*
	* \remark If byte array is empty, the behaviour is undefined
	*/

	inline void ByteArray::PopBack()
	{
		Erase(end() - 1);
	}

	/*!
	* \brief Erases the first element
	*
	* \remark If byte array is empty, the behaviour is undefined
	*/

	inline void ByteArray::PopFront()
	{
		Erase(begin());
	}

	/*!
	* \brief Prepends the content of raw memory
	*
	* \param buffer Constant pointer to raw memory
	* \param n Size that can be read
	*
	* \remark If preallocated space of buffer is less than the size, the behaviour is undefined
	*
	* \see Insert
	*/

	inline ByteArray::iterator ByteArray::Prepend(const void* buffer, size_type n)
	{
		return Insert(begin(), buffer, n);
	}

	/*!
	* \brief Prepends another array of bytes
	*
	* \param other ByteArray to add
	*
	* \see Insert
	*/

	inline ByteArray::iterator ByteArray::Prepend(const ByteArray& other)
	{
		return Insert(begin(), other);
	}

	/*!
	* \brief Pushes the byte at the end
	*
	* \param byte Byte to add
	*/

	inline void ByteArray::PushBack(const value_type byte)
	{
		m_array.push_back(byte);
	}

	/*!
	* \brief Pushes the byte at the beginning
	*
	* \param byte Byte to add
	*/

	inline void ByteArray::PushFront(const value_type byte)
	{
		m_array.insert(begin(), 1, byte);
	}

	/*!
	* \brief Reserves enough memory for the buffer size
	*
	* \param bufferSize Size of the buffer to allocate
	*
	* \remark If bufferSize is smaller than the old one, nothing is done
	*/

	inline void ByteArray::Reserve(size_type bufferSize)
	{
		m_array.reserve(bufferSize);
	}

	/*!
	* \brief Resizes the string
	* \return A reference to this
	*
	* \param newSize Target size
	*/

	inline void ByteArray::Resize(size_type newSize)
	{
		m_array.resize(newSize);
	}

	/*!
	* \brief Resizes the string
	* \return A reference to this
	*
	* \param newSize Target size
	* \param byte Byte to add if newSize is greather than actual size
	*/

	inline void ByteArray::Resize(size_type newSize, const value_type byte)
	{
		m_array.resize(newSize, byte);
	}

	/*!
	* \brief Releases the excedent memory
	*/

	inline void ByteArray::ShrinkToFit()
	{
		m_array.shrink_to_fit();
	}

	/*!
	* \brief Swaps the content with the other byte array
	*
	* \param other Other byte array to swap with
	*/

	inline void ByteArray::Swap(ByteArray& other)
	{
		m_array.swap(other.m_array);
	}

	/*!
	* \brief Gives a string representation in base 16
	* \return String in base 16
	*/

	inline String ByteArray::ToHex() const
	{
		std::size_t length = m_array.size() * 2;

		String hexOutput(length, '\0');
		for (std::size_t i = 0; i < m_array.size(); ++i)
			std::sprintf(&hexOutput[i * 2], "%02x", m_array[i]);

		return hexOutput;
	}

	/*!
	* \brief Gives a string representation
	* \return String where each byte is converted to char
	*/

	inline String ByteArray::ToString() const
	{
		return String(reinterpret_cast<const char*>(GetConstBuffer()), GetSize());
	}

	/*!
	* \brief Returns an iterator pointing to the beggining of the string
	* \return Beggining of the string
	*/

	inline ByteArray::iterator ByteArray::begin() noexcept
	{
		return m_array.begin();
	}

	/*!
	* \brief Returns an iterator pointing to the beggining of the string
	* \return Beggining of the string
	*/

	inline ByteArray::const_iterator ByteArray::begin() const noexcept
	{
		return m_array.begin();
	}

	/*!
	* \brief Returns a constant iterator pointing to the beggining of the string
	* \return Beggining of the string
	*/

	inline ByteArray::const_iterator ByteArray::cbegin() const noexcept
	{
		return m_array.cbegin();
	}

	/*!
	* \brief Returns a constant iterator pointing to the end of the string
	* \return End of the string
	*/

	inline ByteArray::const_iterator ByteArray::cend() const noexcept
	{
		return m_array.cend();
	}

	/*!
	* \brief Returns a constant reversed iterator pointing to the beggining of the string
	* \return Beggining of the string
	*/

	inline ByteArray::const_reverse_iterator ByteArray::crbegin() const noexcept
	{
		return m_array.crbegin();
	}

	/*!
	* \brief Returns a constant reversed iterator pointing to the end of the string
	* \return End of the string
	*/

	inline ByteArray::const_reverse_iterator ByteArray::crend() const noexcept
	{
		return m_array.crend();
	}

	/*!
	* \brief Checks whether the byte array is empty
	* \return true if byte array is empty
	*/

	inline bool ByteArray::empty() const noexcept
	{
		return m_array.empty();
	}

	/*!
	* \brief Returns an iterator pointing to the end of the string
	* \return End of the string
	*/

	inline ByteArray::iterator ByteArray::end() noexcept
	{
		return m_array.end();
	}

	/*!
	* \brief Returns an iterator pointing to the end of the string
	* \return End of the string
	*/

	inline ByteArray::const_iterator ByteArray::end() const noexcept
	{
		return m_array.end();
	}

	/*!
	* \brief Returns a reversed iterator pointing to the beggining of the string
	* \return Beggining of the string
	*/

	inline ByteArray::reverse_iterator ByteArray::rbegin() noexcept
	{
		return m_array.rbegin();
	}

	/*!
	* \brief Returns a reversed iterator pointing to the beggining of the string
	* \return Beggining of the string
	*/

	inline ByteArray::const_reverse_iterator ByteArray::rbegin() const noexcept
	{
		return m_array.rbegin();
	}

	/*!
	* \brief Returns a reversed iterator pointing to the end of the string
	* \return End of the string
	*/

	inline ByteArray::reverse_iterator ByteArray::rend() noexcept
	{
		return m_array.rend();
	}

	/*!
	* \brief Gets the size of the byte array
	* \return Size of the byte array
	*/

	inline ByteArray::size_type ByteArray::size() const noexcept
	{
		return GetSize();
	}

	/*!
	* \brief Gets the ith byte
	* \return A reference to the byte at the ith position
	*
	* \remark Produces a NazaraAssert if pos is greather than the size
	*/

	inline ByteArray::reference ByteArray::operator[](size_type pos)
	{
		NazaraAssert(pos < GetSize(), "Index out of range");

		return m_array[pos];
	}

	/*!
	* \brief Gets the ith byte
	* \return A constant reference to the byte at the ith position
	*
	* \remark Produces a NazaraAssert if pos is greather than the size
	*/

	inline ByteArray::const_reference ByteArray::operator[](size_type pos) const
	{
		NazaraAssert(pos < GetSize(), "Index out of range");

		return m_array[pos];
	}

	/*!
	* \brief Concatenates the byte array to another
	* \return ByteArray which is the result of the concatenation
	*
	* \param other ByteArray to add
	*/

	inline ByteArray ByteArray::operator+(const ByteArray& other) const
	{
		ByteArray tmp(*this);
		tmp += other;

		return tmp;
	}

	/*!
	* \brief Concatenates the byte array to this byte array
	* \return A reference to this
	*
	* \param other ByteArray to add
	*/

	inline ByteArray& ByteArray::operator+=(const ByteArray& other)
	{
		Append(other);

		return *this;
	}

	/*!
	* \brief Checks whether the first byte array is equal to the second byte array
	* \return true if it is the case
	*
	* \param rhs ByteArray to compare with
	*/

	inline bool ByteArray::operator==(const ByteArray& rhs) const
	{
		return m_array == rhs.m_array;
	}

	/*!
	* \brief Checks whether the first byte array is equal to the second byte array
	* \return false if it is the case
	*
	* \param rhs ByteArray to compare with
	*/

	inline bool ByteArray::operator!=(const ByteArray& rhs) const
	{
		return !operator==(rhs);
	}

	/*!
	* \brief Checks whether the first byte array is less than the second byte array
	* \return true if it is the case
	*
	* \param rhs ByteArray to compare with
	*/

	inline bool ByteArray::operator<(const ByteArray& rhs) const
	{
		return m_array < rhs.m_array;
	}

	/*!
	* \brief Checks whether the first byte array is less or equal than the second byte array
	* \return true if it is the case
	*
	* \param rhs ByteArray to compare with
	*/

	inline bool ByteArray::operator<=(const ByteArray& rhs) const
	{
		return m_array <= rhs.m_array;
	}

	/*!
	* \brief Checks whether the first byte array is greater than the second byte array
	* \return true if it is the case
	*
	* \param rhs ByteArray to compare with
	*/

	inline bool ByteArray::operator>(const ByteArray& rhs) const
	{
		return m_array > rhs.m_array;
	}

	/*!
	* \brief Checks whether the first byte array is greater or equal than the second byte array
	* \return true if it is the case
	*
	* \param rhs ByteArray to compare with
	*/

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
	/*!
	* \brief Swaps two byte arrays, specialisation of std
	*
	* \param lhs First byte array
	* \param rhs Second byte array
	*/

	inline void swap(Nz::ByteArray& lhs, Nz::ByteArray& rhs)
	{
		lhs.Swap(rhs);
	}
}

#include <Nazara/Core/DebugOff.hpp>

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BYTEARRAY_HPP
#define NAZARA_BYTEARRAY_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Hashable.hpp>
#include <Nazara/Core/String.hpp>
#include <memory>
#include <vector>

class NzAbstractHash;

class NAZARA_API NzByteArray : public NzHashable
{
	using Container = std::vector<nzUInt8>;

	public:
		// types:
		using value_type = typename Container::value_type;
		using allocator_type = typename Container::allocator_type;
		using size_type = typename Container::size_type;
		using difference_type = typename Container::difference_type;
		using reference = typename Container::reference;
		using const_reference = typename Container::const_reference;
		using pointer = typename Container::pointer;
		using const_pointer = typename Container::const_pointer;
		using iterator = typename Container::iterator;
		using const_iterator = typename Container::const_iterator;
		using reverse_iterator = typename Container::reverse_iterator;
		using const_reverse_iterator = typename Container::const_reverse_iterator;

		// iterators:
		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		iterator end() noexcept;
		const_iterator end() const noexcept;
		reverse_iterator rbegin() noexcept;
		const_reverse_iterator rbegin() const noexcept;
		reverse_iterator rend() noexcept;
		const_reverse_iterator rend() const noexcept;
		const_iterator cbegin() const noexcept;
		const_iterator cend() const noexcept;
		const_reverse_iterator crbegin() const noexcept;
		const_reverse_iterator crend() const noexcept;

		// construct/destroy:
		NzByteArray();
		explicit NzByteArray(size_type n);
		NzByteArray(const void* buffer, size_type n);
		NzByteArray(size_type n, const value_type value);
		template <class InputIterator>
		NzByteArray(InputIterator first, InputIterator last);
		NzByteArray(const NzByteArray& other);
		NzByteArray(NzByteArray&& other);
		virtual ~NzByteArray();

		iterator Append(const void* buffer, size_type size);
		iterator Append(const NzByteArray& other);
		template <class InputIterator>
		void Assign(InputIterator first, InputIterator last);
		void Assign(size_type n, const value_type value);

		reference Back();
		const_reference Back() const;

		void Clear(bool keepBuffer = false);

		pointer data();
		const_pointer data() const;

		bool empty() const noexcept;
		iterator Erase(const_iterator pos);
		iterator Erase(const_iterator first, const_iterator last);

		reference Front();
		const_reference Front() const;

		iterator Insert(const_iterator pos, const void* buffer, size_type n);
		iterator Insert(const_iterator pos, const NzByteArray& other);
		iterator Insert(const_iterator pos, size_type n, const value_type byte);
		template <class InputIterator>
		iterator Insert(const_iterator pos, InputIterator first, InputIterator last);
		bool IsEmpty() const noexcept;

		allocator_type GetAllocator() const;
		pointer GetBuffer();
		size_type GetCapacity() const noexcept;
		const_pointer GetConstBuffer() const;
		size_type GetSize() const noexcept;

		size_type MaxSize() const noexcept;

		// operators:
		reference operator[](size_type pos);
		const_reference operator[](size_type pos) const;
		NzByteArray& operator=(const NzByteArray& array);
		NzByteArray& operator=(NzByteArray&& array);
		NzByteArray operator+(const NzByteArray& array) const;
		NzByteArray& operator+=(const NzByteArray& array);

		bool operator==(const NzByteArray& rhs) const;
		bool operator!=(const NzByteArray& rhs) const;
		bool operator<(const NzByteArray& rhs) const;
		bool operator<=(const NzByteArray& rhs) const;
		bool operator>(const NzByteArray& rhs) const;
		bool operator>=(const NzByteArray& rhs) const;

		void PopBack();
		void PopFront();
		iterator Prepend(const void* buffer, size_type size);
		iterator Prepend(const NzByteArray& other);
		void PushBack(const value_type byte);
		void PushFront(const value_type byte);

		void Reserve(size_type bufferSize);
		void Resize(size_type newSize);
		void Resize(size_type newSize, const value_type byte);

		size_type size() const noexcept;
		void ShrinkToFit();
		NzByteArray SubArray(const_iterator startPos, const_iterator endPos) const;
		void Swap(NzByteArray& other);

		NzString ToString() const;

	private:

		bool FillHash(NzAbstractHash* hash) const;

		Container m_array;
};

NAZARA_API std::ostream& operator<<(std::ostream& out, const NzByteArray& byteArray);

namespace std
{
	NAZARA_API void swap(NzByteArray& lhs, NzByteArray& rhs);
}

template <class InputIterator>
NzByteArray::NzByteArray(InputIterator first, InputIterator last) :
m_array(first, last)
{
}

template <class InputIterator>
void NzByteArray::Assign(InputIterator first, InputIterator last)
{
	m_array.assign(first, last);
}

template <class InputIterator>
NzByteArray::iterator NzByteArray::Insert(const_iterator pos, InputIterator first, InputIterator last)
{
	return m_array.insert(pos, first, last);
}

#endif // NAZARA_BYTEARRAY_HPP

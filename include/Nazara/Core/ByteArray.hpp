// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BYTEARRAY_HPP
#define NAZARA_BYTEARRAY_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/String.hpp>
#include <vector>

namespace Nz
{
	class AbstractHash;

	class NAZARA_CORE_API ByteArray
	{
		using Container = std::vector<UInt8>;

		public:
			// types:
			using allocator_type = Container::allocator_type;
			using const_iterator = Container::const_iterator;
			using const_reference = Container::const_reference;
			using const_pointer = Container::const_pointer;
			using const_reverse_iterator = Container::const_reverse_iterator;
			using difference_type = Container::difference_type;
			using pointer = Container::pointer;
			using iterator = Container::iterator;
			using reference = Container::reference;
			using reverse_iterator = Container::reverse_iterator;
			using size_type = Container::size_type;
			using value_type = Container::value_type;

			// construct/destroy:
			inline ByteArray() = default;
			inline explicit ByteArray(size_type n);
			inline ByteArray(const void* buffer, size_type n);
			inline ByteArray(size_type n, value_type value);
			template <class InputIterator> ByteArray(InputIterator first, InputIterator last);
			ByteArray(const ByteArray& other) = default;
			ByteArray(ByteArray&& other) = default;
			~ByteArray() = default;

			inline iterator Append(const void* buffer, size_type size);
			inline iterator Append(const ByteArray& other);
			template <class InputIterator> void Assign(InputIterator first, InputIterator last);
			inline void Assign(size_type n, value_type value);

			inline reference Back();
			inline const_reference Back() const;

			inline void Clear(bool keepBuffer = false);

			inline iterator Erase(const_iterator pos);
			inline iterator Erase(const_iterator first, const_iterator last);

			inline reference Front();
			inline const_reference Front() const;

			inline allocator_type GetAllocator() const;
			inline pointer GetBuffer();
			inline size_type GetCapacity() const noexcept;
			inline const_pointer GetConstBuffer() const;
			inline size_type GetMaxSize() const noexcept;
			inline size_type GetSize() const noexcept;
			inline ByteArray GetSubArray(const_iterator startPos, const_iterator endPos) const;

			inline iterator Insert(const_iterator pos, const void* buffer, size_type n);
			inline iterator Insert(const_iterator pos, const ByteArray& other);
			inline iterator Insert(const_iterator pos, size_type n, value_type byte);
			template <class InputIterator> iterator Insert(const_iterator pos, InputIterator first, InputIterator last);
			inline bool IsEmpty() const noexcept;

			inline void PopBack();
			inline void PopFront();
			inline iterator Prepend(const void* buffer, size_type size);
			inline iterator Prepend(const ByteArray& other);
			inline void PushBack(value_type byte);
			inline void PushFront(value_type byte);

			inline void Reserve(size_type bufferSize);
			inline void Resize(size_type newSize);
			inline void Resize(size_type newSize, value_type byte);

			inline void ShrinkToFit();
			inline void Swap(ByteArray& other);

			String ToHex() const;
			inline String ToString() const;

			// STL interface
			inline iterator begin() noexcept;
			inline const_iterator begin() const noexcept;
			inline bool empty() const noexcept;
			inline iterator end() noexcept;
			inline const_iterator end() const noexcept;
			inline reverse_iterator rbegin() noexcept;
			inline const_reverse_iterator rbegin() const noexcept;
			inline reverse_iterator rend() noexcept;
			inline const_reverse_iterator rend() const noexcept;
			inline const_iterator cbegin() const noexcept;
			inline const_iterator cend() const noexcept;
			inline const_reverse_iterator crbegin() const noexcept;
			inline const_reverse_iterator crend() const noexcept;
			inline size_type size() const noexcept;

			// Operators
			NAZARA_CORE_API friend std::ostream& operator<<(std::ostream& out, const Nz::ByteArray& byteArray);

			inline reference operator[](size_type pos);
			inline const_reference operator[](size_type pos) const;
			inline ByteArray& operator=(const ByteArray& array) = default;
			inline ByteArray& operator=(ByteArray&& array) = default;
			inline ByteArray operator+(const ByteArray& array) const;
			inline ByteArray& operator+=(const ByteArray& array);

			inline bool operator==(const ByteArray& rhs) const;
			inline bool operator!=(const ByteArray& rhs) const;
			inline bool operator<(const ByteArray& rhs) const;
			inline bool operator<=(const ByteArray& rhs) const;
			inline bool operator>(const ByteArray& rhs) const;
			inline bool operator>=(const ByteArray& rhs) const;

		private:
			Container m_array;
	};

	inline bool HashAppend(AbstractHash* hash, const ByteArray& byteArray);
}

namespace std
{
	void swap(Nz::ByteArray& lhs, Nz::ByteArray& rhs);
}

#include <Nazara/Core/ByteArray.inl>

#endif // NAZARA_BYTEARRAY_HPP

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STACKARRAY_HPP
#define NAZARA_STACKARRAY_HPP

#include <Nazara/Core/MemoryHelper.hpp>

#ifdef NAZARA_ALLOCA_SUPPORT
	#define NazaraStackArray(T, size) Nz::StackArray<T>(static_cast<T*>(NAZARA_ALLOCA((size) * sizeof(T))), size)
	#define NazaraStackArrayNoInit(T, size) Nz::StackArray<T>(static_cast<T*>(NAZARA_ALLOCA((size) * sizeof(T))), size, typename Nz::StackArray<T>::NoInitTag())
#else
	#define NazaraStackArray(T, size) Nz::StackArray<T>(static_cast<T*>(Nz::OperatorNew((size) * sizeof(T))), size)
	#define NazaraStackArrayNoInit(T, size) Nz::StackArray<T>(static_cast<T*>(Nz::OperatorNew((size) * sizeof(T))), size, typename Nz::StackArray<T>::NoInitTag())
#endif

#include <cstddef>
#include <iterator>

namespace Nz
{
	template<typename T>
	class StackArray
	{
		public:
			struct NoInitTag {};

			using value_type = T;
			using const_iterator = const value_type*;
			using const_pointer = const value_type*;
			using const_reference = const value_type&;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			using difference_type = std::ptrdiff_t;
			using iterator = value_type*;
			using pointer = value_type*;
			using reference = value_type&;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using size_type = std::size_t;

			StackArray(T* stackMemory, std::size_t size);
			StackArray(T* stackMemory, std::size_t size, NoInitTag);
			~StackArray();

			reference back();
			const_reference back() const;

			iterator begin() noexcept;
			const_iterator begin() const noexcept;

			const_iterator cbegin() const noexcept;
			const_iterator cend() const noexcept;
			const_reverse_iterator crbegin() const noexcept;
			const_reverse_iterator crend() const noexcept;

			T* data() noexcept;
			const T* data() const noexcept;

			bool empty() const noexcept;

			iterator end() noexcept;
			const_iterator end() const noexcept;

			void fill(const T& value);

			reference front() noexcept;
			const_reference front() const noexcept;

			size_type max_size() const noexcept;

			reverse_iterator rbegin() noexcept;
			const_reverse_iterator rbegin() const noexcept;

			reverse_iterator rend() noexcept;
			const_reverse_iterator rend() const noexcept;

			size_type size() const noexcept;

			reference operator[](size_type pos);
			const_reference operator[](size_type pos) const;

		private:
			std::size_t m_size;
			T* m_ptr;
	};

}

#include <Nazara/Core/StackArray.inl>

#endif // NAZARA_STACKARRAY_HPP

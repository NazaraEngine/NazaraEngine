// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYHELPER_HPP
#define NAZARA_MEMORYHELPER_HPP

#if defined(NAZARA_COMPILER_MSVC) || defined(NAZARA_COMPILER_MINGW)

#include <malloc.h>

// with MSVC, using alloca with a size of zero returns a valid pointer
#define NAZARA_ALLOCA(size) _alloca(size)
#define NAZARA_ALLOCA_SUPPORT

#elif defined(NAZARA_COMPILER_CLANG) || defined(NAZARA_COMPILER_GCC) || defined(NAZARA_COMPILER_INTEL)
#include <alloca.h>

// with Clang/GCC, using alloca with a size of zero does nothing good
#define NAZARA_ALLOCA(size) alloca(((size) > 0) ? (size) : 1)
#define NAZARA_ALLOCA_SUPPORT

#endif

#ifdef NAZARA_ALLOCA_SUPPORT
	#define NazaraStackAllocation(T, size) Nz::StackArray<T>(static_cast<T*>(NAZARA_ALLOCA((size) * sizeof(T))), size)
	#define NazaraStackAllocationNoInit(T, size) Nz::StackArray<T>(static_cast<T*>(NAZARA_ALLOCA((size) * sizeof(T))), size, Nz::NoInitTag())
#else
	#define NazaraStackAllocation(T, size) Nz::StackArray<T>(static_cast<T*>(Nz::OperatorNew((size) * sizeof(T))), size)
	#define NazaraStackAllocationNoInit(T, size) Nz::StackArray<T>(static_cast<T*>(Nz::OperatorNew((size) * sizeof(T))), size, Nz::NoInitTag())
#endif

#include <cstddef>
#include <iterator>

namespace Nz
{
	void OperatorDelete(void* ptr);
	void* OperatorNew(std::size_t size);

	template<typename T, typename... Args>
	T* PlacementNew(T* ptr, Args&&... args);

	template<typename T>
	void PlacementDestroy(T* ptr);

	struct NoInitTag {};

	template<typename T>
	class StackArray
	{
		public:
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

#include <Nazara/Core/MemoryHelper.inl>

#endif // NAZARA_MEMORYHELPER_HPP

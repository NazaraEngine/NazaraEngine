// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STACKVECTOR_HPP
#define NAZARA_STACKVECTOR_HPP

#include <Nazara/Core/MemoryHelper.hpp>

#ifdef NAZARA_ALLOCA_SUPPORT
	#define NazaraStackVector(T, capacity) Nz::StackVector<T>(static_cast<T*>(NAZARA_ALLOCA((capacity) * sizeof(T))), capacity)
#else
	#define NazaraStackVector(T, capacity) Nz::StackVector<T>(static_cast<T*>(Nz::OperatorNew((capacity) * sizeof(T))), capacity)
#endif

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace Nz
{
	template<typename T>
	class StackVector
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

			StackVector(T* stackMemory, std::size_t capacity);
			~StackVector();

			reference back();
			const_reference back() const;

			iterator begin() noexcept;
			const_iterator begin() const noexcept;

			size_type capacity() const noexcept;

			void clear() noexcept;

			const_iterator cbegin() const noexcept;
			const_iterator cend() const noexcept;
			const_reverse_iterator crbegin() const noexcept;
			const_reverse_iterator crend() const noexcept;

			T* data() noexcept;
			const T* data() const noexcept;

			template<typename... Args>
			iterator emplace(const_iterator pos, Args&&... args);

			template<typename... Args>
			reference emplace_back(Args&&... args);

			bool empty() const noexcept;

			iterator end() noexcept;
			const_iterator end() const noexcept;

			iterator erase(const_iterator pos);
			iterator erase(const_iterator first, const_iterator last);

			reference front() noexcept;
			const_reference front() const noexcept;

			iterator insert(const_iterator pos, const T& value);
			iterator insert(const_iterator pos, T&& value);

			size_type max_size() const noexcept;

			reference push_back(const T& value) noexcept(std::is_nothrow_copy_constructible<T>::value);
			reference push_back(T&& value) noexcept(std::is_nothrow_move_constructible<T>::value);

			void pop_back();

			void resize(size_type count);
			void resize(size_type count, const value_type& value);

			reverse_iterator rbegin() noexcept;
			const_reverse_iterator rbegin() const noexcept;

			reverse_iterator rend() noexcept;
			const_reverse_iterator rend() const noexcept;

			size_type size() const noexcept;

			reference operator[](size_type pos);
			const_reference operator[](size_type pos) const;

		private:
			std::size_t m_capacity;
			std::size_t m_size;
			T* m_ptr;
	};

}

#include <Nazara/Core/StackVector.inl>

#endif // NAZARA_STACKVECTOR_HPP

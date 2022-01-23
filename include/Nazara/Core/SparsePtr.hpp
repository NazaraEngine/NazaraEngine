// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_SPARSEPTR_HPP
#define NAZARA_CORE_SPARSEPTR_HPP

///FIXME: Is SparsePtr a really good name for this class ?

#include <Nazara/Prerequisites.hpp>
#include <cstddef>
#include <type_traits>

namespace Nz
{
	template<typename T>
	class SparsePtr
	{
		public:
			using BytePtr = std::conditional_t<std::is_const<T>::value, const UInt8*, UInt8*>;
			using VoidPtr = std::conditional_t<std::is_const<T>::value, const void*, void*>;

			SparsePtr();
			SparsePtr(T* ptr);
			SparsePtr(VoidPtr ptr, int stride);
			SparsePtr(VoidPtr ptr, std::size_t stride);
			template<typename U> SparsePtr(const SparsePtr<U>& ptr);
			SparsePtr(const SparsePtr& ptr) = default;
			~SparsePtr() = default;

			VoidPtr GetPtr() const;
			int GetStride() const;

			void Reset();
			void Reset(T* ptr);
			void Reset(VoidPtr ptr, int stride);
			void Reset(const SparsePtr& ptr);
			template<typename U> void Reset(const SparsePtr<U>& ptr);

			void SetPtr(VoidPtr ptr);
			void SetStride(int stride);

			explicit operator bool() const;
			explicit operator T*() const;
			T& operator*() const;
			T* operator->() const;
			template<typename U> T& operator[](U index) const;

			SparsePtr& operator=(const SparsePtr& ptr) = default;

			template<typename U> SparsePtr operator+(U count) const;
			template<typename U> SparsePtr operator-(U count) const;
			std::ptrdiff_t operator-(const SparsePtr& ptr) const;

			template<typename U> SparsePtr& operator+=(U count);
			template<typename U> SparsePtr& operator-=(U count);

			SparsePtr& operator++();
			SparsePtr operator++(int);

			SparsePtr& operator--();
			SparsePtr operator--(int);

			bool operator==(const SparsePtr& ptr) const;
			bool operator!=(const SparsePtr& ptr) const;
			bool operator<(const SparsePtr& ptr) const;
			bool operator>(const SparsePtr& ptr) const;
			bool operator<=(const SparsePtr& ptr) const;
			bool operator>=(const SparsePtr& ptr) const;

		private:
			BytePtr m_ptr;
			int m_stride;
	};
}

#include <Nazara/Core/SparsePtr.inl>

#endif // NAZARA_CORE_SPARSEPTR_HPP

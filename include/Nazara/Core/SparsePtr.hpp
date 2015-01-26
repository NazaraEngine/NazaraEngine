// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPARSEPTR_HPP
#define NAZARA_SPARSEPTR_HPP

///FIXME: Est-ce que SparsePtr est vraiment le meilleur nom pour cette classe ?

#include <Nazara/Prerequesites.hpp>
#include <cstddef>
#include <type_traits>

template<typename T>
class NzSparsePtr
{
	public:
		using BytePtr = typename std::conditional<std::is_const<T>::value, const nzUInt8*, nzUInt8*>::type;
		using VoidPtr = typename std::conditional<std::is_const<T>::value, const void*, void*>::type;

		NzSparsePtr();
		NzSparsePtr(T* ptr);
		NzSparsePtr(VoidPtr ptr, int stride);
		template<typename U> NzSparsePtr(const NzSparsePtr<U>& ptr);
		NzSparsePtr(const NzSparsePtr& ptr) = default;
		~NzSparsePtr() = default;

		VoidPtr GetPtr() const;
		int GetStride() const;

		void Reset();
		void Reset(T* ptr);
		void Reset(VoidPtr ptr, int stride);
		void Reset(const NzSparsePtr& ptr);
		template<typename U> void Reset(const NzSparsePtr<U>& ptr);

		void SetPtr(VoidPtr ptr);
		void SetStride(int stride);

		operator bool() const;
		operator T*() const;
		T& operator*() const;
		T* operator->() const;
		T& operator[](int index) const;

		NzSparsePtr operator+(int count) const;
		NzSparsePtr operator+(unsigned int count) const;
		NzSparsePtr operator-(int count) const;
		NzSparsePtr operator-(unsigned int count) const;
		std::ptrdiff_t operator-(const NzSparsePtr& ptr) const;

		NzSparsePtr& operator+=(int count);
		NzSparsePtr& operator-=(int count);

		NzSparsePtr& operator++();
		NzSparsePtr operator++(int);

		NzSparsePtr& operator--();
		NzSparsePtr operator--(int);

		bool operator==(const NzSparsePtr& ptr) const;
		bool operator!=(const NzSparsePtr& ptr) const;
		bool operator<(const NzSparsePtr& ptr) const;
		bool operator>(const NzSparsePtr& ptr) const;
		bool operator<=(const NzSparsePtr& ptr) const;
		bool operator>=(const NzSparsePtr& ptr) const;

		NzSparsePtr& operator=(const NzSparsePtr& ptr) = default;

	private:
		BytePtr m_ptr;
		int m_stride;
};

#include <Nazara/Core/SparsePtr.inl>

#endif // NAZARA_SPARSEPTR_HPP

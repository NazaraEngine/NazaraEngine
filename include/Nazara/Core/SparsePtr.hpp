// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPARSEPTR_HPP
#define NAZARA_SPARSEPTR_HPP

///FIXME: Est-ce que SparsePtr est vraiment le meilleur nom pour cette classe ?

#include <Nazara/Prerequesites.hpp>

template<typename T>
class NzSparsePtr
{
	public:
		NzSparsePtr();
		NzSparsePtr(void* ptr, unsigned int stride);
		NzSparsePtr(const NzSparsePtr& ptr) = default;
		~NzSparsePtr() = default;

		void* Get() const;
		unsigned int GetStride() const;
		void Set(void* ptr);
		void SetStride(unsigned int stride);

		T& operator*() const;
		T& operator->() const;
		T& operator[](unsigned int index) const;

		NzSparsePtr operator+(unsigned int count) const;
		NzSparsePtr operator-(unsigned int count) const;

		NzSparsePtr& operator+=(unsigned int count);
		NzSparsePtr& operator-=(unsigned int count);

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
		nzUInt8* m_ptr;
		unsigned int m_stride;
};

#include <Nazara/Core/SparsePtr.inl>

#endif // NAZARA_SPARSEPTR_HPP

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <iterator>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename T>
	SparsePtr<T>::SparsePtr()
	{
		Reset();
	}

	template<typename T>
	SparsePtr<T>::SparsePtr(T* ptr)
	{
		Reset(ptr);
	}

	template<typename T>
	SparsePtr<T>::SparsePtr(VoidPtr ptr, int stride)
	{
		Reset(ptr, stride);
	}

	template<typename T>
	template<typename U>
	SparsePtr<T>::SparsePtr(const SparsePtr<U>& ptr)
	{
		Reset(ptr);
	}

	template<typename T>
	typename SparsePtr<T>::VoidPtr SparsePtr<T>::GetPtr() const
	{
		return m_ptr;
	}

	template<typename T>
	int SparsePtr<T>::GetStride() const
	{
		return m_stride;
	}

	template<typename T>
	void SparsePtr<T>::Reset()
	{
		SetPtr(nullptr);
		SetStride(0);
	}

	template<typename T>
	void SparsePtr<T>::Reset(T* ptr)
	{
		SetPtr(ptr);
		SetStride(sizeof(T));
	}

	template<typename T>
	void SparsePtr<T>::Reset(VoidPtr ptr, int stride)
	{
		SetPtr(ptr);
		SetStride(stride);
	}

	template<typename T>
	void SparsePtr<T>::Reset(const SparsePtr& ptr)
	{
		SetPtr(ptr.GetPtr());
		SetStride(ptr.GetStride());
	}

	template<typename T>
	template<typename U>
	void SparsePtr<T>::Reset(const SparsePtr<U>& ptr)
	{
		static_assert(std::is_convertible<U*, T*>::value, "Source type pointer cannot be implicitely converted to target type pointer");

		SetPtr(static_cast<T*>(ptr.GetPtr()));
		SetStride(ptr.GetStride());
	}

	template<typename T>
	void SparsePtr<T>::SetPtr(VoidPtr ptr)
	{
		m_ptr = reinterpret_cast<BytePtr>(ptr);
	}

	template<typename T>
	void SparsePtr<T>::SetStride(int stride)
	{
		m_stride = stride;
	}

	template<typename T>
	SparsePtr<T>::operator bool() const
	{
		return m_ptr != nullptr;
	}

	template<typename T>
	SparsePtr<T>::operator T*() const
	{
		return reinterpret_cast<T*>(m_ptr);
	}

	template<typename T>
	T& SparsePtr<T>::operator*() const
	{
		return *reinterpret_cast<T*>(m_ptr);
	}

	template<typename T>
	T* SparsePtr<T>::operator->() const
	{
		return reinterpret_cast<T*>(m_ptr);
	}

	template<typename T>
	T& SparsePtr<T>::operator[](int index) const
	{
		return *reinterpret_cast<T*>(m_ptr + index*m_stride);
	}

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator+(int count) const
	{
		return SparsePtr(m_ptr + count*m_stride, m_stride);
	}

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator+(unsigned int count) const
	{
		return SparsePtr(m_ptr + count*m_stride, m_stride);
	}

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator-(int count) const
	{
		return SparsePtr(m_ptr - count*m_stride, m_stride);
	}

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator-(unsigned int count) const
	{
		return SparsePtr(m_ptr - count*m_stride, m_stride);
	}

	template<typename T>
	std::ptrdiff_t SparsePtr<T>::operator-(const SparsePtr& ptr) const
	{
		return (m_ptr - ptr.m_ptr)/m_stride;
	}

	template<typename T>
	SparsePtr<T>& SparsePtr<T>::operator+=(int count)
	{
		m_ptr += count*m_stride;

		return *this;
	}

	template<typename T>
	SparsePtr<T>& SparsePtr<T>::operator-=(int count)
	{
		m_ptr -= count*m_stride;

		return *this;
	}

	template<typename T>
	SparsePtr<T>& SparsePtr<T>::operator++()
	{
		m_ptr += m_stride;

		return *this;
	}

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator++(int)
	{
		// On fait une copie de l'objet
		SparsePtr tmp(*this);

		// On modifie l'objet
		operator++();

		// On retourne la copie
		return tmp;
	}

	template<typename T>
	SparsePtr<T>& SparsePtr<T>::operator--()
	{
		m_ptr -= m_stride;
		return *this;
	}

	template<typename T>
	SparsePtr<T> SparsePtr<T>::operator--(int)
	{
		// On fait une copie de l'objet
		SparsePtr tmp(*this);

		// On modifie l'objet
		operator--();

		// On retourne la copie
		return tmp;
	}

	template<typename T>
	bool SparsePtr<T>::operator==(const SparsePtr& ptr) const
	{
		return m_ptr == ptr.m_ptr;
	}

	template<typename T>
	bool SparsePtr<T>::operator!=(const SparsePtr& ptr) const
	{
		return m_ptr != ptr.m_ptr;
	}

	template<typename T>
	bool SparsePtr<T>::operator<(const SparsePtr& ptr) const
	{
		return m_ptr < ptr.m_ptr;
	}

	template<typename T>
	bool SparsePtr<T>::operator>(const SparsePtr& ptr) const
	{
		return m_ptr > ptr.m_ptr;
	}

	template<typename T>
	bool SparsePtr<T>::operator<=(const SparsePtr& ptr) const
	{
		return m_ptr <= ptr.m_ptr;
	}

	template<typename T>
	bool SparsePtr<T>::operator>=(const SparsePtr& ptr) const
	{
		return m_ptr >= ptr.m_ptr;
	}
}

namespace std
{
	template<typename T>
	struct iterator_traits<Nz::SparsePtr<T>>
	{
		using difference_type = ptrdiff_t;
		using iterator_category = random_access_iterator_tag;
		using reference = const T&;
		using pointer = const T*;
		using value_type = T;
	};
}

#include <Nazara/Core/DebugOff.hpp>

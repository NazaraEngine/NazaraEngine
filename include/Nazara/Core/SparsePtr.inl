// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

template<typename T>
NzSparsePtr<T>::NzSparsePtr()
{
	Reset();
}

template<typename T>
NzSparsePtr<T>::NzSparsePtr(T* ptr)
{
	Reset(ptr);
}

template<typename T>
NzSparsePtr<T>::NzSparsePtr(VoidPtr ptr, int stride)
{
	Reset(ptr, stride);
}

template<typename T>
template<typename U>
NzSparsePtr<T>::NzSparsePtr(const NzSparsePtr<U>& ptr)
{
	Reset(ptr);
}

template<typename T>
typename NzSparsePtr<T>::VoidPtr NzSparsePtr<T>::GetPtr() const
{
	return m_ptr;
}

template<typename T>
int NzSparsePtr<T>::GetStride() const
{
	return m_stride;
}

template<typename T>
void NzSparsePtr<T>::Reset()
{
	SetPtr(nullptr);
	SetStride(0);
}

template<typename T>
void NzSparsePtr<T>::Reset(T* ptr)
{
	SetPtr(ptr);
	SetStride(sizeof(T));
}

template<typename T>
void NzSparsePtr<T>::Reset(VoidPtr ptr, int stride)
{
	SetPtr(ptr);
	SetStride(stride);
}

template<typename T>
void NzSparsePtr<T>::Reset(const NzSparsePtr& ptr)
{
	SetPtr(ptr.GetPtr());
	SetStride(ptr.GetStride());
}

template<typename T>
template<typename U>
void NzSparsePtr<T>::Reset(const NzSparsePtr<U>& ptr)
{
	static_assert(std::is_convertible<U*, T*>::value, "Source type pointer cannot be implicitely converted to target type pointer");

	SetPtr(static_cast<T*>(ptr.GetPtr()));
	SetStride(ptr.GetStride());
}

template<typename T>
void NzSparsePtr<T>::SetPtr(VoidPtr ptr)
{
	m_ptr = reinterpret_cast<BytePtr>(ptr);
}

template<typename T>
void NzSparsePtr<T>::SetStride(int stride)
{
	m_stride = stride;
}

template<typename T>
NzSparsePtr<T>::operator bool() const
{
	return m_ptr != nullptr;
}

template<typename T>
NzSparsePtr<T>::operator T*() const
{
	return reinterpret_cast<T*>(m_ptr);
}

template<typename T>
T& NzSparsePtr<T>::operator*() const
{
	return *reinterpret_cast<T*>(m_ptr);
}

template<typename T>
T* NzSparsePtr<T>::operator->() const
{
	return reinterpret_cast<T*>(m_ptr);
}

template<typename T>
T& NzSparsePtr<T>::operator[](int index) const
{
	return *reinterpret_cast<T*>(m_ptr + index*m_stride);
}

template<typename T>
NzSparsePtr<T> NzSparsePtr<T>::operator+(int count) const
{
	return NzSparsePtr(m_ptr + count*m_stride, m_stride);
}

template<typename T>
NzSparsePtr<T> NzSparsePtr<T>::operator+(unsigned int count) const
{
	return NzSparsePtr(m_ptr + count*m_stride, m_stride);
}

template<typename T>
NzSparsePtr<T> NzSparsePtr<T>::operator-(int count) const
{
	return NzSparsePtr(m_ptr - count*m_stride, m_stride);
}

template<typename T>
NzSparsePtr<T> NzSparsePtr<T>::operator-(unsigned int count) const
{
	return NzSparsePtr(m_ptr - count*m_stride, m_stride);
}

template<typename T>
std::ptrdiff_t NzSparsePtr<T>::operator-(const NzSparsePtr& ptr) const
{
	return (m_ptr - ptr.m_ptr)/m_stride;
}

template<typename T>
NzSparsePtr<T>& NzSparsePtr<T>::operator+=(int count)
{
	m_ptr += count*m_stride;

	return *this;
}

template<typename T>
NzSparsePtr<T>& NzSparsePtr<T>::operator-=(int count)
{
	m_ptr -= count*m_stride;

	return *this;
}

template<typename T>
NzSparsePtr<T>& NzSparsePtr<T>::operator++()
{
	m_ptr += m_stride;

	return *this;
}

template<typename T>
NzSparsePtr<T> NzSparsePtr<T>::operator++(int)
{
	// On fait une copie de l'objet
	NzSparsePtr tmp(*this);

	// On modifie l'objet
	operator++();

	// On retourne la copie
	return tmp;
}

template<typename T>
NzSparsePtr<T>& NzSparsePtr<T>::operator--()
{
	m_ptr -= m_stride;
	return *this;
}

template<typename T>
NzSparsePtr<T> NzSparsePtr<T>::operator--(int)
{
	// On fait une copie de l'objet
	NzSparsePtr tmp(*this);

	// On modifie l'objet
	operator--();

	// On retourne la copie
	return tmp;
}

template<typename T>
bool NzSparsePtr<T>::operator==(const NzSparsePtr& ptr) const
{
	return m_ptr == ptr.m_ptr;
}

template<typename T>
bool NzSparsePtr<T>::operator!=(const NzSparsePtr& ptr) const
{
	return m_ptr != ptr.m_ptr;
}

template<typename T>
bool NzSparsePtr<T>::operator<(const NzSparsePtr& ptr) const
{
	return m_ptr < ptr.m_ptr;
}

template<typename T>
bool NzSparsePtr<T>::operator>(const NzSparsePtr& ptr) const
{
	return m_ptr > ptr.m_ptr;
}

template<typename T>
bool NzSparsePtr<T>::operator<=(const NzSparsePtr& ptr) const
{
	return m_ptr <= ptr.m_ptr;
}

template<typename T>
bool NzSparsePtr<T>::operator>=(const NzSparsePtr& ptr) const
{
	return m_ptr >= ptr.m_ptr;
}

#include <Nazara/Core/DebugOff.hpp>

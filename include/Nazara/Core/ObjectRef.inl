// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzObjectRef<T>::NzObjectRef() :
m_object(nullptr)
{
}

template<typename T>
NzObjectRef<T>::NzObjectRef(T* object) :
m_object(object)
{
	if (m_object)
		m_object->AddReference();
}

template<typename T>
NzObjectRef<T>::NzObjectRef(const NzObjectRef& ref) :
m_object(ref.m_object)
{
	if (m_object)
		m_object->AddReference();
}

template<typename T>
template<typename U>
NzObjectRef<T>::NzObjectRef(const NzObjectRef<U>& ref) :
NzObjectRef(ref.Get())
{
}

template<typename T>
NzObjectRef<T>::NzObjectRef(NzObjectRef&& ref) noexcept :
m_object(ref.m_object)
{
	ref.m_object = nullptr; // On vole la référence
}

template<typename T>
NzObjectRef<T>::~NzObjectRef()
{
	if (m_object)
		m_object->RemoveReference();
}

template<typename T>
T* NzObjectRef<T>::Get() const
{
	return m_object;
}

template<typename T>
bool NzObjectRef<T>::IsValid() const
{
	return m_object != nullptr;
}

template<typename T>
T* NzObjectRef<T>::Release()
{
	T* object = m_object;
	m_object = nullptr;

	return object;
}

template<typename T>
bool NzObjectRef<T>::Reset(T* object)
{
	bool destroyed = false;
	if (m_object != object)
	{
		if (m_object)
			destroyed = m_object->RemoveReference();

		m_object = object;
		if (m_object)
			m_object->AddReference();
	}

	return destroyed;
}

template<typename T>
NzObjectRef<T>& NzObjectRef<T>::Swap(NzObjectRef& ref)
{
	std::swap(m_object, ref.m_object);

	return *this;
}

template<typename T>
NzObjectRef<T>::operator bool() const
{
	return IsValid();
}

template<typename T>
NzObjectRef<T>::operator T*() const
{
	return m_object;
}

template<typename T>
T* NzObjectRef<T>::operator->() const
{
	return m_object;
}

template<typename T>
NzObjectRef<T>& NzObjectRef<T>::operator=(T* object)
{
	Reset(object);

	return *this;
}

template<typename T>
NzObjectRef<T>& NzObjectRef<T>::operator=(const NzObjectRef& ref)
{
	Reset(ref.m_object);

	return *this;
}

template<typename T>
template<typename U>
NzObjectRef<T>& NzObjectRef<T>::operator=(const NzObjectRef<U>& ref)
{
	static_assert(std::is_convertible<U*, T*>::value, "U is not implicitly convertible to T");

	Reset(ref.Get());

	return *this;
}

template<typename T>
NzObjectRef<T>& NzObjectRef<T>::operator=(NzObjectRef&& ref) noexcept
{
	Reset();

	std::swap(m_object, ref.m_object);

	return *this;
}

#include <Nazara/Core/DebugOff.hpp>

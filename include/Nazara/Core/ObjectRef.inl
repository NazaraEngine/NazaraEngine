// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzObjectRef<T>::NzObjectRef() :
m_resource(nullptr)
{
}

template<typename T>
NzObjectRef<T>::NzObjectRef(T* resource) :
m_resource(resource)
{
	if (m_resource)
		m_resource->AddReference();
}

template<typename T>
NzObjectRef<T>::NzObjectRef(const NzObjectRef& ref) :
m_resource(ref.m_resource)
{
	if (m_resource)
		m_resource->AddReference();
}

template<typename T>
NzObjectRef<T>::NzObjectRef(NzObjectRef&& ref) noexcept :
m_resource(ref.m_resource)
{
	ref.m_resource = nullptr; // On vole la référence
}

template<typename T>
NzObjectRef<T>::~NzObjectRef()
{
	if (m_resource)
		m_resource->RemoveReference();
}

template<typename T>
bool NzObjectRef<T>::IsValid() const
{
	return m_resource != nullptr;
}

template<typename T>
T* NzObjectRef<T>::Release()
{
	T* resource = m_resource;
	m_resource = nullptr;

	return resource;
}

template<typename T>
bool NzObjectRef<T>::Reset(T* resource)
{
	bool destroyed = false;
	if (m_resource != resource)
	{
		if (m_resource)
		{
			destroyed = m_resource->RemoveReference();
			m_resource = nullptr;
		}

		m_resource = resource;
		if (m_resource)
			m_resource->AddReference();
	}

	return destroyed;
}

template<typename T>
NzObjectRef<T>& NzObjectRef<T>::Swap(NzObjectRef& ref)
{
	std::swap(m_resource, ref.m_resource);

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
	return m_resource;
}

template<typename T>
T* NzObjectRef<T>::operator->() const
{
	return m_resource;
}

template<typename T>
NzObjectRef<T>& NzObjectRef<T>::operator=(T* resource)
{
	Reset(resource);

	return *this;
}

template<typename T>
NzObjectRef<T>& NzObjectRef<T>::operator=(const NzObjectRef& ref)
{
	Reset(ref.m_resource);

	return *this;
}

template<typename T>
NzObjectRef<T>& NzObjectRef<T>::operator=(NzObjectRef&& ref) noexcept
{
	Reset();

	std::swap(m_resource, ref.m_resource);

	return *this;
}

#include <Nazara/Core/DebugOff.hpp>

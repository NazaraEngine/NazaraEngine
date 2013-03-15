// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Initializer.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzResourceRef<T>::NzResourceRef(T* resource) :
m_resource(resource)
{
	if (m_resource)
		m_resource->AddResourceReference();
}

template<typename T>
NzResourceRef<T>::NzResourceRef(const NzResourceRef& ref) :
m_resource(ref.m_resource)
{
	if (m_resource)
		m_resource->AddResourceReference();
}

template<typename T>
NzResourceRef<T>::NzResourceRef(NzResourceRef&& ref) :
m_resource(ref.m_resource)
{
	ref.m_resource = nullptr; // On vole la référence
}

template<typename T>
NzResourceRef<T>::~NzResourceRef()
{
	if (m_resource)
		m_resource->RemoveResourceReference();
}

template<typename T>
bool NzResourceRef<T>::IsValid() const
{
	return m_resource != nullptr;
}

template<typename T>
T* NzResourceRef<T>::Release()
{
	T* resource = m_resource;
	m_resource = nullptr;

	return resource;
}

template<typename T>
bool NzResourceRef<T>::Reset(T* resource)
{
	bool destroyed = false;
	if (m_resource)
	{
		destroyed = m_resource->RemoveResourceReference();
		m_resource = nullptr;
	}

	m_resource = resource;
	if (m_resource)
		m_resource->AddResourceReference();

	return destroyed;
}

template<typename T>
NzResourceRef<T>& NzResourceRef<T>::Swap(NzResourceRef& ref)
{
	std::swap(m_resource, ref.m_resource);

	return *this;
}

template<typename T>
NzResourceRef<T>::operator bool() const
{
	return IsValid();
}

template<typename T>
NzResourceRef<T>::operator T*() const
{
	return m_resource;
}

template<typename T>
T* NzResourceRef<T>::operator->() const
{
	return m_resource;
}

template<typename T>
NzResourceRef<T>& NzResourceRef<T>::operator=(const NzResourceRef& ref)
{
	if (m_resource != ref.m_resource)
	{
		Release();

		if (ref)
		{
			m_resource = ref.m_resource;
			m_resource->AddResourceReference();
		}
	}

	return *this;
}

template<typename T>
NzResourceRef<T>& NzResourceRef<T>::operator=(NzResourceRef&& ref)
{
	Release();

	std::swap(m_resource, ref.m_resource);

	return *this;
}

#include <Nazara/Core/DebugOff.hpp>

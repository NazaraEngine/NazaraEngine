// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

template<typename T>
NzResourceListenerWrapper<T>::NzResourceListenerWrapper(NzResourceListener* listener, int index, T* resource) :
m_resource(nullptr),
m_listener(listener),
m_index(index)
{
	Reset(resource);
}

template<typename T>
NzResourceListenerWrapper<T>::NzResourceListenerWrapper(const NzResourceListenerWrapper& listener) :
m_resource(nullptr),
m_listener(listener.m_listener),
m_index(listener.m_index)
{
	Reset(listener.m_resource);
}

template<typename T>
NzResourceListenerWrapper<T>::NzResourceListenerWrapper(NzResourceListenerWrapper&& listener) :
m_resource(listener.m_resource),
m_listener(listener.m_listener),
m_index(listener.m_index)
{
	listener.m_resource = nullptr;
}

template<typename T>
NzResourceListenerWrapper<T>::~NzResourceListenerWrapper()
{
	Reset(nullptr);
}

template<typename T>
bool NzResourceListenerWrapper<T>::IsValid() const
{
	return m_resource != nullptr;
}

template<typename T>
void NzResourceListenerWrapper<T>::Reset(T* resource)
{
	if (resource)
		resource->AddResourceListener(m_listener, m_index);

	if (m_resource)
		m_resource->RemoveResourceListener(m_listener);

	m_resource = resource;
}

template<typename T>
NzResourceListenerWrapper<T>::operator bool() const
{
	return IsValid();
}

template<typename T>
NzResourceListenerWrapper<T>::operator T*() const
{
	return m_resource;
}

template<typename T>
T* NzResourceListenerWrapper<T>::operator->() const
{
	return m_resource;
}

template<typename T>
NzResourceListenerWrapper<T>& NzResourceListenerWrapper<T>::operator=(T* resource)
{
	Reset(resource);

	return *this;
}

template<typename T>
NzResourceListenerWrapper<T>& NzResourceListenerWrapper<T>::operator=(const NzResourceListenerWrapper& listener)
{
	m_index = listener.m_index;
	m_listener = listener.m_listener;
	Reset(listener.m_resource);

	return *this;
}

template<typename T>
NzResourceListenerWrapper<T>& NzResourceListenerWrapper<T>::operator=(NzResourceListenerWrapper&& listener)
{
	Reset();

	m_index = listener.m_index;
	m_listener = listener.m_listener;
	m_resource = listener.m_resource;

	listener.m_resource = nullptr;

	return *this;
}

#include <Nazara/Core/DebugOff.hpp>

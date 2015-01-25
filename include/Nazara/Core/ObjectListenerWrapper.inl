// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

template<typename T>
NzObjectListenerWrapper<T>::NzObjectListenerWrapper(NzObjectListener* listener, int index, T* object) :
m_object(nullptr),
m_listener(listener),
m_index(index)
{
	Reset(object);
}

template<typename T>
NzObjectListenerWrapper<T>::NzObjectListenerWrapper(const NzObjectListenerWrapper& listener) :
m_object(nullptr),
m_listener(listener.m_listener),
m_index(listener.m_index)
{
	Reset(listener.m_object);
}

template<typename T>
NzObjectListenerWrapper<T>::NzObjectListenerWrapper(NzObjectListenerWrapper&& listener) :
m_object(listener.m_object),
m_listener(listener.m_listener),
m_index(listener.m_index)
{
	listener.m_object = nullptr;
}

template<typename T>
NzObjectListenerWrapper<T>::~NzObjectListenerWrapper()
{
	Reset(nullptr);
}

template<typename T>
bool NzObjectListenerWrapper<T>::IsValid() const
{
	return m_object != nullptr;
}

template<typename T>
void NzObjectListenerWrapper<T>::Reset(T* object)
{
	if (object)
		object->AddObjectListener(m_listener, m_index);

	if (m_object)
		m_object->RemoveObjectListener(m_listener);

	m_object = object;
}

template<typename T>
NzObjectListenerWrapper<T>::operator bool() const
{
	return IsValid();
}

template<typename T>
NzObjectListenerWrapper<T>::operator T*() const
{
	return m_object;
}

template<typename T>
T* NzObjectListenerWrapper<T>::operator->() const
{
	return m_object;
}

template<typename T>
NzObjectListenerWrapper<T>& NzObjectListenerWrapper<T>::operator=(T* object)
{
	Reset(object);

	return *this;
}

template<typename T>
NzObjectListenerWrapper<T>& NzObjectListenerWrapper<T>::operator=(const NzObjectListenerWrapper& listener)
{
	m_index = listener.m_index;
	m_listener = listener.m_listener;
	Reset(listener.m_object);

	return *this;
}

template<typename T>
NzObjectListenerWrapper<T>& NzObjectListenerWrapper<T>::operator=(NzObjectListenerWrapper&& listener)
{
	Reset();

	m_index = listener.m_index;
	m_listener = listener.m_listener;
	m_object = listener.m_object;

	listener.m_object = nullptr;

	return *this;
}

#include <Nazara/Core/DebugOff.hpp>

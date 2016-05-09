// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <algorithm>
#include <type_traits>
#include <utility>

namespace Nz
{
	template<typename T>
	HandledObject<T>::HandledObject(const HandledObject& object)
	{
		// Don't copy anything, we're a copy of the object, we have no handle right now
	}

	template<typename T>
	HandledObject<T>::HandledObject(HandledObject&& object) :
	m_handles(std::move(object.m_handles))
	{
		for (ObjectHandle<T>* handle : m_handles)
			handle->OnObjectMoved(static_cast<T*>(this));
	}

	template<typename T>
	HandledObject<T>::~HandledObject()
	{
		UnregisterAllHandles();
	}

	template<typename T>
	ObjectHandle<T> HandledObject<T>::CreateHandle()
	{
		return ObjectHandle<T>(static_cast<T*>(this));
	}

	template<typename T>
	HandledObject<T>& HandledObject<T>::operator=(const HandledObject& object)
	{
		// Nothing to do
		return *this; 
	}

	template<typename T>
	HandledObject<T>& HandledObject<T>::operator=(HandledObject&& object)
	{
		m_handles = std::move(object.m_handles);
		for (ObjectHandle<T>* handle : m_handles)
			handle->OnObjectMoved(static_cast<T*>(this));

		return *this;
	}

	template<typename T>
	void HandledObject<T>::RegisterHandle(ObjectHandle<T>* handle)
	{
		///DOC: Un handle ne doit être enregistré qu'une fois, des erreurs se produisent s'il l'est plus d'une fois
		m_handles.push_back(handle);
	}

	template<typename T>
	void HandledObject<T>::UnregisterAllHandles()
	{
		// Tell every handle we got destroyed, to null them
		for (ObjectHandle<T>* handle : m_handles)
			handle->OnObjectDestroyed();

		m_handles.clear();
	}

	template<typename T>
	void HandledObject<T>::UnregisterHandle(ObjectHandle<T>* handle) noexcept
	{
		///DOC: Un handle ne doit être libéré qu'une fois, et doit faire partie de la liste, sous peine de crash
		auto it = std::find(m_handles.begin(), m_handles.end(), handle);
		NazaraAssert(it != m_handles.end(), "Handle not registered");

		// Swap and pop idiom, more efficient than vector::erase
		std::swap(*it, m_handles.back());
		m_handles.pop_back();
	}

	template<typename T>
	void HandledObject<T>::UpdateHandle(ObjectHandle<T>* oldHandle, ObjectHandle<T>* newHandle) noexcept
	{
		auto it = std::find(m_handles.begin(), m_handles.end(), oldHandle);
		NazaraAssert(it != m_handles.end(), "Handle not registered");

		// Simply update the handle
		*it = newHandle;
	}
}

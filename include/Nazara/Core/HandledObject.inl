// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ObjectHandle.hpp>
#include <algorithm>
#include <type_traits>
#include <utility>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::HandledObject<T>
	* \brief Core class that represents a handled object
	*/

	/*!
	* \brief Constructs a HandledObject object by assignation
	*
	* \param object HandledObject to assign into this
	*/
	template<typename T>
	HandledObject<T>::HandledObject(const HandledObject& object)
	{
		NazaraUnused(object);
		// Don't copy anything, we're a copy of the object, we have no handle right now
	}

	/*!
	* \brief Constructs a HandledObject object by move semantic
	*
	* \param object HandledObject to move into this
	*/
	template<typename T>
	HandledObject<T>::HandledObject(HandledObject&& object) noexcept :
	m_handles(std::move(object.m_handles))
	{
		for (ObjectHandle<T>* handle : m_handles)
			handle->OnObjectMoved(static_cast<T*>(this));
	}

	/*!
	* \brief Destructs the object and calls UnregisterAllHandles
	*
	* \see UnregisterAllHandles
	*/
	template<typename T>
	HandledObject<T>::~HandledObject()
	{
		UnregisterAllHandles();
	}

	/*!
	* \brief Creates a ObjectHandle for this
	* \return ObjectHandle to this
	*/
	template<typename T>
	ObjectHandle<T> HandledObject<T>::CreateHandle()
	{
		return ObjectHandle<T>(static_cast<T*>(this));
	}

	/*!
	* \brief Sets the reference of the HandledObject with the handle from another
	* \return A reference to this
	*
	* \param object The other HandledObject
	*/
	template<typename T>
	HandledObject<T>& HandledObject<T>::operator=(const HandledObject& object)
	{
		NazaraUnused(object);

		// Nothing to do
		return *this; 
	}

	/*!
	* \brief Moves the HandledObject into this
	* \return A reference to this
	*
	* \param object HandledObject to move in this
	*/
	template<typename T>
	HandledObject<T>& HandledObject<T>::operator=(HandledObject&& object) noexcept
	{
		UnregisterAllHandles();

		m_handles = std::move(object.m_handles);
		for (ObjectHandle<T>* handle : m_handles)
			handle->OnObjectMoved(static_cast<T*>(this));

		return *this;
	}

	/*!
	* \brief Registers a handle
	*
	* \param handle Handle to register
	*
	* \remark One handle can only be registered once, errors can occur if it's more than once
	*/
	template<typename T>
	void HandledObject<T>::RegisterHandle(ObjectHandle<T>* handle)
	{
		m_handles.push_back(handle);
	}

	/*!
	* \brief Unregisters all handles
	*/
	template<typename T>
	void HandledObject<T>::UnregisterAllHandles() noexcept
	{
		// Tell every handle we got destroyed, to null them
		for (ObjectHandle<T>* handle : m_handles)
			handle->OnObjectDestroyed();

		m_handles.clear();
	}

	/*!
	* \brief Unregisters a handle
	*
	* \param handle Handle to unregister
	*
	* \remark One handle can only be unregistered once, crash can occur if it's more than once
	* \remark Produces a NazaraAssert if handle not registered
	*/
	template<typename T>
	void HandledObject<T>::UnregisterHandle(ObjectHandle<T>* handle) noexcept
	{
		auto it = std::find(m_handles.begin(), m_handles.end(), handle);
		NazaraAssert(it != m_handles.end(), "Handle not registered");

		// Swap and pop idiom, more efficient than vector::erase
		std::swap(*it, m_handles.back());
		m_handles.pop_back();
	}

	/*!
	* \brief Updates one handle with another
	*
	* \param oldHandle Old handle to replace
	* \param newHandle New handle to take place
	*
	* \remark Produces a NazaraAssert if handle not registered
	*/
	template<typename T>
	void HandledObject<T>::UpdateHandle(ObjectHandle<T>* oldHandle, ObjectHandle<T>* newHandle) noexcept
	{
		auto it = std::find(m_handles.begin(), m_handles.end(), oldHandle);
		NazaraAssert(it != m_handles.end(), "Handle not registered");

		// Simply update the handle
		*it = newHandle;
	}
}

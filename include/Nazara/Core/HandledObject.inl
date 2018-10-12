// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Core/Error.hpp>
#include <algorithm>
#include <cassert>
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
	m_handleData(std::move(object.m_handleData))
	{
		if (m_handleData)
			m_handleData->object = static_cast<T*>(this);
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

		m_handleData = std::move(object.m_handleData);

		if (m_handleData)
			m_handleData->object = static_cast<T*>(this);

		return *this;
	}

	/*!
	* \brief Unregisters all handles
	*/
	template<typename T>
	void HandledObject<T>::UnregisterAllHandles() noexcept
	{
		if (m_handleData)
		{
			m_handleData->object = nullptr;
			m_handleData.reset();
		}
	}

	template<typename T>
	std::shared_ptr<const Detail::HandleData> HandledObject<T>::GetHandleData()
	{
		if (!m_handleData)
			InitHandleData();

		return std::shared_ptr<const Detail::HandleData>(m_handleData);
	}

	template<typename T>
	void HandledObject<T>::InitHandleData()
	{
		assert(!m_handleData);

		m_handleData = std::make_shared<Detail::HandleData>();
		m_handleData->object = static_cast<T*>(this);
	}
}

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	/*!
	* \brief Constructs an ListenerComponent object by default
	*/

	inline ListenerComponent::ListenerComponent() :
	m_isActive(true)
	{
	}

	/*!
	* \brief Checks whether the listener is activated
	* \param true If it is the case
	*/

	inline bool ListenerComponent::IsActive() const
	{
		return m_isActive;
	}

	/*!
	* \brief Enables the listener
	*
	* \param active Should the listener be active
	*/

	inline void ListenerComponent::SetActive(bool active)
	{
		m_isActive = active;
	}
}

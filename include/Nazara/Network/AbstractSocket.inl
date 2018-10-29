// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Gets the last error
	* \return Socket error
	*/

	inline SocketError AbstractSocket::GetLastError() const
	{
		return m_lastError;
	}

	/*!
	* \brief Gets the internal socket handle
	* \return Socket handle
	*/

	inline SocketHandle AbstractSocket::GetNativeHandle() const
	{
		return m_handle;
	}

	/*!
	* \brief Gets the internal state
	* \return Socket state
	*/

	inline SocketState AbstractSocket::GetState() const
	{
		return m_state;
	}

	/*!
	* \brief Gets the internal type
	* \return Socket type
	*/

	inline SocketType AbstractSocket::GetType() const
	{
		return m_type;
	}

	/*!
	* \brief Checks whether the blocking is enabled
	* \return true If successful
	*/

	inline bool AbstractSocket::IsBlockingEnabled() const
	{
		return m_isBlockingEnabled;
	}

	/*!
	* \brief Updates the state of the socket
	*
	* \param newState Next state for the socket
	*/

	inline void AbstractSocket::UpdateState(SocketState newState)
	{
		if (m_state != newState)
		{
			SocketState oldState = m_state;
			m_state = newState;
			OnStateChanged(this, oldState, newState);
		}
	}
}

#include <Nazara/Network/DebugOff.hpp>

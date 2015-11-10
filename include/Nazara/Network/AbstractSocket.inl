// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline SocketError AbstractSocket::GetLastError() const
	{
		return m_lastError;
	}

	inline SocketHandle AbstractSocket::GetNativeHandle() const
	{
		return m_handle;
	}

	inline SocketState AbstractSocket::GetState() const
	{
		return m_state;
	}

	inline SocketType AbstractSocket::GetType() const
	{
		return m_type;
	}

	inline bool AbstractSocket::IsBlockingEnabled() const
	{
		return m_isBlockingEnabled;
	}

	inline void AbstractSocket::UpdateState(SocketState newState)
	{
		if (m_state != newState)
		{
			OnStateChange(this, m_state);
			m_state = newState;
		}
	}
}

#include <Nazara/Network/DebugOff.hpp>

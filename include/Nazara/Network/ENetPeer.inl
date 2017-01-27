// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/ENetPeer.hpp>
#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline const IpAddress& ENetPeer::GetAddress() const
	{
		return m_address;
	}

	inline void ENetPeer::ChangeState(ENetPeerState state)
	{
		if (state == ENetPeerState::Connected || state == ENetPeerState::DisconnectLater)
			OnConnect();
		else
			OnDisconnect();

		m_state = state;
	}
}

#include <Nazara/Network/DebugOff.hpp>

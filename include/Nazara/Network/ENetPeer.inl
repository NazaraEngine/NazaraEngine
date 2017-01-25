// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/ENetPeer.hpp>
#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline void ENetPeer::ChangeState(ENetPeerState state)
	{
		if (state == ENetPeerState::Connected || state == ENetPeerState::DisconnectLater)
			OnConnect();
		else
			OnDisconnect();

		m_state = state;
	}

	inline void ENetPeer::DispatchState(ENetPeerState state)
	{
		ChangeState(state);

		m_host->AddToDispatchQueue(this);
	}
}

#include <Nazara/Network/DebugOff.hpp>

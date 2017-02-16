// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/ENetPeer.hpp>
#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline ENetPeer::ENetPeer(ENetHost* host, UInt16 peerId) :
	m_host(host),
	m_incomingSessionID(0xFF),
	m_outgoingSessionID(0xFF),
	m_incomingPeerID(peerId)
	{
		Reset();
	}

	inline const IpAddress& ENetPeer::GetAddress() const
	{
		return m_address;
	}

	inline UInt32 ENetPeer::GetMtu() const
	{
		return m_mtu;
	}

	inline UInt32 ENetPeer::GetPacketThrottleAcceleration() const
	{
		return m_packetThrottleAcceleration;
	}

	inline UInt32 ENetPeer::GetPacketThrottleDeceleration() const
	{
		return m_packetThrottleDeceleration;
	}

	inline UInt32 ENetPeer::GetPacketThrottleInterval() const
	{
		return m_packetThrottleInterval;
	}

	inline UInt16 ENetPeer::GetPeerId() const
	{
		return m_incomingPeerID;
	}

	inline ENetPeerState ENetPeer::GetState() const
	{
		return m_state;
	}

	inline bool ENetPeer::HasPendingCommands()
	{
		return m_outgoingReliableCommands.empty() && m_outgoingUnreliableCommands.empty() && m_sentReliableCommands.empty();
	}

	inline bool ENetPeer::IsConnected() const
	{
		return m_state == ENetPeerState::Connected || m_state == ENetPeerState::DisconnectLater;
	}

	inline void ENetPeer::ChangeState(ENetPeerState state)
	{
		if (state == ENetPeerState::Connected || state == ENetPeerState::DisconnectLater)
			OnConnect();
		else
			OnDisconnect();

		m_state = state;
	}

	inline void ENetPeer::QueueOutgoingCommand(ENetProtocol& command)
	{
		QueueOutgoingCommand(command, ENetPacketRef(), 0, 0);
	}
}

#include <Nazara/Network/DebugOff.hpp>

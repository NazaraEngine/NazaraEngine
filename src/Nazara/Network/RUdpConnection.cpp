// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/RUdpConnection.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	RUdpConnection::RUdpConnection() :
	m_peerIterator(0),
	m_forceAckSendTime(10'000), //< 10ms
	m_pingInterval(1'000'000), //< 1s
	m_protocol(0x4E4E6574), //< "NNet"
	m_timeBeforePing(500'000), //< 0.5s
	m_timeBeforeTimeOut(10'000'000), //< 10s
	m_currentTime(0),
	m_isSimulationEnabled(false),
	m_shouldAcceptConnections(true)
	{
	}

	bool RUdpConnection::Connect(const IpAddress& remoteAddress)
	{
		NazaraAssert(m_socket.GetState() != SocketState_Bound, "Socket must be bound first");
		NazaraAssert(remoteAddress.IsValid(), "Invalid remote address");
		NazaraAssert(remoteAddress.GetPort() != 0, "Remote address has no port");

		PeerData& client = RegisterPeer(remoteAddress, PeerState_Connecting);
		client.stateData1 = s_randomGenerator();

		NetPacket connectionRequestPacket(NetCode_RequestConnection);
		connectionRequestPacket << client.stateData1;

		EnqueuePacket(client, PacketPriority_Immediate, PacketReliability_Reliable, connectionRequestPacket);
		return true;
	}

	bool RUdpConnection::Connect(const String& hostName, NetProtocol protocol, const String& service, ResolveError* error)
	{
		std::vector<HostnameInfo> results = IpAddress::ResolveHostname(protocol, hostName, service, error);
		if (results.empty())
		{
			m_lastError = SocketError_ResolveError;
			return false;
		}

		IpAddress hostnameAddress;
		for (const HostnameInfo& result : results)
		{
			if (!result.address)
				continue;

			if (result.socketType != SocketType_UDP)
				continue;

			hostnameAddress = result.address;
			break; //< Take first valid address
		}

		return Connect(hostnameAddress);
	}

	bool RUdpConnection::Listen(const IpAddress& address)
	{
		if (!InitSocket(address.GetProtocol()))
			return false;

		return m_socket.Bind(address) == SocketState_Bound;
	}

	bool RUdpConnection::PollMessage(RUdpMessage* message)
	{
		if (m_receivedMessages.empty())
			return false;

		*message = std::move(m_receivedMessages.front());
		m_receivedMessages.pop();
		return true;
	}

	bool RUdpConnection::Send(const IpAddress& peerIp, PacketPriority priority, PacketReliability reliability, const NetPacket& packet)
	{
		auto it = m_peerByIP.find(peerIp);
		if (it == m_peerByIP.end())
			return false; /// Silently fail (probably a disconnected client)

		EnqueuePacket(m_peers[it->second], priority, reliability, packet);
		return true;
	}

	void RUdpConnection::Update()
	{
		m_currentTime = m_clock.GetMicroseconds();

		NetPacket receivedPacket;
		IpAddress senderIp;
		while (m_socket.ReceivePacket(&receivedPacket, &senderIp))
			OnPacketReceived(senderIp, std::move(receivedPacket));

		//for (unsigned int i = m_activeClients.FindFirst(); i != m_activeClients.npos; i = m_activeClients.FindNext(i))
		//{
		//	PeerData& clientData = m_peers[i];

		CallOnExit resetIterator([this] () { m_peerIterator = m_peers.size(); });

		for (m_peerIterator = 0; m_peerIterator < m_peers.size(); ++m_peerIterator)
		{
			PeerData& peer = m_peers[m_peerIterator];

			UInt32 timeSinceLastPacket = static_cast<UInt32>(m_currentTime - peer.lastPacketTime);
			if (timeSinceLastPacket > m_timeBeforeTimeOut)
			{
				DisconnectPeer(peer.index);
				continue;
			}
			else if (timeSinceLastPacket > m_timeBeforePing)
			{
				if (m_currentTime - peer.lastPingTime > m_pingInterval)
				{
					NetPacket pingPacket(NetCode_Ping);
					EnqueuePacket(peer, PacketPriority_Low, PacketReliability_Unreliable, pingPacket);
				}
			}

			if (peer.state == PeerState_WillAck && m_currentTime - peer.stateData1 > m_forceAckSendTime)
			{
				NetPacket acknowledgePacket(NetCode_Acknowledge);
				EnqueuePacket(peer, PacketPriority_Low, PacketReliability_Reliable, acknowledgePacket);
			}

			for (unsigned int priority = PacketPriority_Highest; priority <= PacketPriority_Lowest; ++priority)
			{
				std::vector<PendingPacket>& pendingPackets = peer.pendingPackets[priority];
				for (PendingPacket& packetData : pendingPackets)
					SendPacket(peer, std::move(packetData));

				pendingPackets.clear();
			}

			auto it = peer.pendingAckQueue.begin();
			while (it != peer.pendingAckQueue.end())
			{
				if (m_currentTime - it->timeSent > 2 * peer.roundTripTime)
				{
					OnPacketLost(peer, std::move(*it));
					it = peer.pendingAckQueue.erase(it);
				}
				else
					++it;
			}
		}
		//m_activeClients.Reset();
	}

	void RUdpConnection::DisconnectPeer(std::size_t peerIndex)
	{
		PeerData& peer = m_peers[peerIndex];
		NazaraNotice(m_socket.GetBoundAddress().ToString() + ": " + peer.address.ToString() + " has been disconnected due to time-out");

		OnPeerDisconnected(this, peer.address);

		// Remove from IP lookup table
		m_peerByIP.erase(peer.address);

		// Can we safely "remove" this slot?
		if (m_peerIterator >= m_peers.size() - 1 || peerIndex > m_peerIterator)
		{
			// Yes we can
			PeerData& newSlot = m_peers[peerIndex];
			newSlot = std::move(m_peers.back());
			newSlot.index = peerIndex; //< Update the moved slot index before resizing (in case it's the last one)
		}
		else
		{
			// Nope, let's be tricky
			PeerData& current = m_peers[m_peerIterator];
			PeerData& newSlot = m_peers[peerIndex];

			newSlot = std::move(current);
			newSlot.index = peerIndex; //< Update the moved slot index

			current = std::move(m_peers.back());
			current.index = m_peerIterator; //< Update the moved slot index

			--m_peerIterator;
		}

		// Pop the last entry (from where we moved our slot)
		m_peers.pop_back();
	}

	void RUdpConnection::EnqueuePacket(PeerData& peer, PacketPriority priority, PacketReliability reliability, const NetPacket& packet)
	{
		UInt16 protocolBegin = static_cast<UInt16>(m_protocol & 0xFFFF);
		UInt16 protocolEnd = static_cast<UInt16>((m_protocol & 0xFFFF0000) >> 16);

		NetPacket data(packet.GetNetCode(), MessageHeader + packet.GetDataSize() + MessageFooter);
		data << protocolBegin;

		data.GetStream()->SetCursorPos(NetPacket::HeaderSize + MessageHeader);
		data.Write(packet.GetConstData() + NetPacket::HeaderSize, packet.GetDataSize());

		data << protocolEnd;
		EnqueuePacketInternal(peer, priority, reliability, std::move(data));
	}

	void RUdpConnection::EnqueuePacketInternal(PeerData& peer, PacketPriority priority, PacketReliability reliability, NetPacket&& data)
	{
		PendingPacket pendingPacket;
		pendingPacket.data = std::move(data);
		pendingPacket.priority = priority;
		pendingPacket.reliability = reliability;

		peer.pendingPackets[priority].emplace_back(std::move(pendingPacket));
		m_activeClients.UnboundedSet(peer.index);
	}

	bool RUdpConnection::InitSocket(NetProtocol protocol)
	{
		CallOnExit updateLastError([this]
		{
			m_lastError = m_socket.GetLastError();
		});

		if (!m_socket.Create(protocol))
			return false;

		m_socket.EnableBlocking(false);
		return true;
	}

	void RUdpConnection::ProcessAcks(PeerData& peer, SequenceIndex lastAck, UInt32 ackBits)
	{
		auto it = peer.pendingAckQueue.begin();
		while (it != peer.pendingAckQueue.end())
		{
			bool acked = false;
			if (lastAck == it->sequenceId)
				acked = true;
			else if (!IsAckMoreRecent(it->sequenceId, lastAck))
			{
				unsigned int difference = ComputeSequenceDifference(lastAck, it->sequenceId);
				if (difference <= 32)
					acked = (ackBits >> (difference - 1)) & 1;
			}

			if (acked)
			{
				it = peer.pendingAckQueue.erase(it);
			}
			else
				++it;
		}
	}

	RUdpConnection::PeerData& RUdpConnection::RegisterPeer(const IpAddress& address, PeerState state)
	{
		PeerData data;
		data.address = address;
		data.localSequence = 0;
		data.remoteSequence = 0;
		data.index = m_peers.size();
		data.lastPacketTime = m_currentTime;
		data.lastPingTime = m_currentTime;
		data.roundTripTime = 1000000; ///< Okay that's quite a lot
		data.state = state;

		m_activeClients.UnboundedSet(data.index);
		m_peerByIP[address] = data.index;

		m_peers.emplace_back(std::move(data));
		return m_peers.back();
	}

	void RUdpConnection::OnClientRequestingConnection(const IpAddress& address, SequenceIndex sequenceId, UInt64 token)
	{
		// Call hook to check if client should be accepted or not
		OnPeerConnection(this, address);

		PeerData& client = RegisterPeer(address, PeerState_Aknowledged);
		client.remoteSequence = sequenceId;

		/// Acknowledge connection
		NetPacket connectionAcceptedPacket(NetCode_AcknowledgeConnection);
		//connectionAcceptedPacket << address;
		connectionAcceptedPacket << ~token;

		EnqueuePacket(client, PacketPriority_Immediate, PacketReliability_Reliable, connectionAcceptedPacket);
	}

	void RUdpConnection::OnPacketLost(PeerData& peer, PendingAckPacket&& packet)
	{
		//NazaraNotice(m_socket.GetBoundAddress().ToString() + ": Lost packet " + String::Number(packet.sequenceId));

		if (IsReliable(packet.reliability))
			EnqueuePacketInternal(peer, packet.priority, packet.reliability, std::move(packet.data));
	}

	void RUdpConnection::OnPacketReceived(const IpAddress& peerIp, NetPacket&& packet)
	{
		UInt16 protocolBegin;
		UInt16 protocolEnd;
		SequenceIndex sequenceId;
		SequenceIndex lastAck;
		UInt32 ackBits;

		packet.GetStream()->SetCursorPos(packet.GetSize() - MessageFooter);
		packet >> protocolEnd;

		packet.GetStream()->SetCursorPos(NetPacket::HeaderSize);
		packet >> protocolBegin;

		UInt32 protocolId = static_cast<UInt32>(protocolEnd) << 16 | protocolBegin;
		if (protocolId != m_protocol)
			return; ///< Ignore

		packet >> sequenceId >> lastAck >> ackBits;

		auto it = m_peerByIP.find(peerIp);
		if (it == m_peerByIP.end())
		{
			switch (packet.GetNetCode())
			{
				case NetCode_RequestConnection:
				{
					UInt64 token;
					packet >> token;

					NazaraNotice(m_socket.GetBoundAddress().ToString() + ": Received NetCode_RequestConnection from " + peerIp.ToString() + ": " + String::Number(token));
					if (!m_shouldAcceptConnections)
						return; //< Ignore

					OnClientRequestingConnection(peerIp, sequenceId, token);
					break;
				}

				default:
					return; //< Ignore
			}
		}
		else
		{
			PeerData& peer = m_peers[it->second];
			peer.lastPacketTime = m_currentTime;

			if (peer.receivedQueue.find(sequenceId) != peer.receivedQueue.end())
				return; //< Ignore

			if (m_isSimulationEnabled && m_packetLossProbability(s_randomGenerator))
			{
				NazaraNotice(m_socket.GetBoundAddress().ToString() + ": Lost packet " + String::Number(sequenceId) + " from " + peerIp.ToString() + " for simulation purpose");
				return;
			}

			///< Receiving a packet from an acknowledged client means the connection works in both ways
			if (peer.state == PeerState_Aknowledged && packet.GetNetCode() != NetCode_RequestConnection)
			{
				peer.state = PeerState_Connected;
				OnPeerAcknowledged(this, peerIp);
			}

			if (IsAckMoreRecent(sequenceId, peer.remoteSequence))
				peer.remoteSequence = sequenceId;

			ProcessAcks(peer, lastAck, ackBits);

			peer.receivedQueue.insert(sequenceId);

			switch (packet.GetNetCode())
			{
				case NetCode_Acknowledge:
					return; //< Do not switch to will ack mode (to prevent infinite replies, just let's ping/pong do that)

				case NetCode_AcknowledgeConnection:
				{
					if (peer.state == PeerState_Connected)
						break;

					IpAddress externalAddress;
					UInt64 token;
					packet /*>> externalAddress*/ >> token;

					NazaraNotice(m_socket.GetBoundAddress().ToString() + ": Received NetCode_AcknowledgeConnection from " + peerIp.ToString() + ": " + String::Number(token));
					if (token == ~peer.stateData1)
					{
						peer.state = PeerState_Connected;
						OnConnectedToPeer(this);
					}
					else
					{
						NazaraNotice("Received wrong token (" + String::Number(token) + " instead of " + String::Number(~peer.stateData1) + ") from client " + peer.address);
						return; //< Ignore
					}

					break;
				}

				case NetCode_RequestConnection:
					NazaraNotice(m_socket.GetBoundAddress().ToString() + ": Received NetCode_RequestConnection from " + peerIp.ToString());
					return; //< Ignore

				case NetCode_Ping:
				{
					NazaraNotice(m_socket.GetBoundAddress().ToString() + ": Received NetCode_Ping from " + peerIp.ToString());

					NetPacket pongPacket(NetCode_Pong);
					EnqueuePacket(peer, PacketPriority_Low, PacketReliability_Unreliable, pongPacket);
					break;
				}

				case NetCode_Pong:
					NazaraNotice(m_socket.GetBoundAddress().ToString() + ": Received NetCode_Pong from " + peerIp.ToString());
					break;

				default:
				{
					NazaraNotice(m_socket.GetBoundAddress().ToString() + ": Received 0x" + String::Number(packet.GetNetCode(), 16) + " from " + peerIp.ToString());
					RUdpMessage receivedMessage;
					receivedMessage.from = peerIp;
					receivedMessage.data = std::move(packet);

					m_receivedMessages.emplace(std::move(receivedMessage));
					break;
				}
			}

			if (!HasPendingPackets(peer))
			{
				peer.state      = PeerState_WillAck;
				peer.stateData1 = m_currentTime;
			}
		}
	}

	void RUdpConnection::SendPacket(PeerData& peer, PendingPacket&& packet)
	{
		if (peer.state == PeerState_WillAck)
			peer.state = PeerState_Connected;

		SequenceIndex remoteSequence = peer.remoteSequence;

		UInt32 previousAcks = 0;
		for (SequenceIndex ack : peer.receivedQueue)
		{
			if (ack == remoteSequence)
				continue;
			
			unsigned int difference = ComputeSequenceDifference(remoteSequence, ack);
			if (difference <= 32U)
				previousAcks |= (1U << (difference - 1));
		}

		SequenceIndex sequenceId = ++peer.localSequence;

		packet.data.GetStream()->SetCursorPos(NetPacket::HeaderSize + sizeof(UInt16)); ///< Protocol begin has already been filled
		packet.data << sequenceId;
		packet.data << remoteSequence;
		packet.data << previousAcks;

		m_socket.SendPacket(peer.address, packet.data);

		PendingAckPacket pendingAckPacket;
		pendingAckPacket.data = std::move(packet.data);
		pendingAckPacket.priority = packet.priority;
		pendingAckPacket.reliability = packet.reliability;
		pendingAckPacket.sequenceId = sequenceId;
		pendingAckPacket.timeSent = m_currentTime;

		peer.pendingAckQueue.emplace_back(std::move(pendingAckPacket));
	}

	bool RUdpConnection::Initialize()
	{
		std::random_device device;
		s_randomGenerator.seed(device());

		return true;
	}

	void RUdpConnection::Uninitialize()
	{
	}

	std::mt19937_64 RUdpConnection::s_randomGenerator;
}

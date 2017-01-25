#include <Nazara/Network/ENetHost.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Network/ENetPeer.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <Nazara/Network/Debug.hpp>

#define ENET_TIME_OVERFLOW 86400000

#define ENET_TIME_LESS(a, b) ((a) - (b) >= ENET_TIME_OVERFLOW)
#define ENET_TIME_GREATER(a, b) ((b) - (a) >= ENET_TIME_OVERFLOW)
#define ENET_TIME_LESS_EQUAL(a, b) (! ENET_TIME_GREATER (a, b))
#define ENET_TIME_GREATER_EQUAL(a, b) (! ENET_TIME_LESS (a, b))

#define ENET_TIME_DIFFERENCE(a, b) ((a) - (b) >= ENET_TIME_OVERFLOW ? (b) - (a) : (a) - (b))

namespace Nz
{
	/// Temporary
	template<typename T>
	T HostToNet(T value)
	{
		#ifdef NAZARA_LITTLE_ENDIAN
		return SwapBytes(value);
		#else
		return value;
		#endif
	}
	
	/// Temporary
	template<typename T>
	T NetToHost(T value)
	{
		#ifdef NAZARA_LITTLE_ENDIAN
		return SwapBytes(value);
		#else
		return value;
		#endif
	}


	void ENetHost::Broadcast(UInt8 channelId, ENetPacketFlags flags, NetPacket&& packet)
	{
		ENetPacket* enetPacket = m_packetPool.New<ENetPacket>();
		enetPacket->flags = flags;
		enetPacket->data = std::move(packet);
		enetPacket->owner = &m_packetPool;

		for (ENetPeer& peer : m_peers)
		{
			if (peer.m_state != ENetPeerState::Connected)
				continue;

			peer.Send(channelId, enetPacket);
		}
	}

	bool ENetHost::Connect(const IpAddress& remoteAddress, std::size_t channelCount, UInt32 data)
	{
		NazaraAssert(remoteAddress.IsValid(), "Invalid remote address");
		NazaraAssert(remoteAddress.GetPort() != 0, "Remote address has no port");

		std::size_t peerId;
		for (peerId = 0; peerId < m_peers.size(); ++peerId)
		{
			if (m_peers[peerId].m_state == ENetPeerState::Disconnected)
				break;
		}

		if (peerId >= m_peers.size())
		{
			NazaraError("Insufficient peers");
			return false;
		}

		m_channelLimit = Clamp<std::size_t>(channelCount, ENetConstants::ENetProtocol_MinimumChannelCount, ENetConstants::ENetProtocol_MaximumChannelCount);

		UInt32 windowSize;
		if (m_outgoingBandwidth == 0)
			windowSize = ENetProtocol_MaximumWindowSize;
		else
			windowSize = (m_outgoingBandwidth / ENetConstants::ENetPeer_WindowSizeScale) * ENetProtocol_MinimumWindowSize;

		ENetPeer& peer = m_peers[peerId];
		peer.InitOutgoing(channelCount, remoteAddress, ++m_randomSeed, windowSize);

		ENetProtocol command;
		command.header.command = ENetProtocolCommand_Connect | ENetProtocolFlag_Acknowledge;
		command.header.channelID = 0xFF;

		command.connect.channelCount = HostToNet(static_cast<UInt32>(channelCount));
		command.connect.connectID = peer.m_connectID;
		command.connect.data = HostToNet(data);
		command.connect.incomingBandwidth = HostToNet(m_incomingBandwidth);
		command.connect.incomingSessionID = peer.m_incomingSessionID;
		command.connect.mtu = HostToNet(peer.m_mtu);
		command.connect.outgoingBandwidth = HostToNet(m_outgoingBandwidth);
		command.connect.outgoingPeerID = HostToNet(peer.m_incomingPeerID);
		command.connect.outgoingSessionID = peer.m_outgoingSessionID;
		command.connect.packetThrottleAcceleration = HostToNet(peer.m_packetThrottleAcceleration);
		command.connect.packetThrottleDeceleration = HostToNet(peer.m_packetThrottleDeceleration);
		command.connect.packetThrottleInterval = HostToNet(peer.m_packetThrottleInterval);
		command.connect.windowSize = HostToNet(peer.m_windowSize);

		peer.QueueOutgoingCommand(command, nullptr, 0, 0);

		return true;
	}

	bool ENetHost::Connect(const String& hostName, NetProtocol protocol, const String& service, ResolveError* error, std::size_t channelCount, UInt32 data)
	{
		std::vector<HostnameInfo> results = IpAddress::ResolveHostname(protocol, hostName, service, error);
		if (results.empty())
			return false;

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

		return Connect(hostnameAddress, channelCount, data);
	}

	bool ENetHost::Create(const IpAddress& address, std::size_t peerCount, std::size_t channelCount)
	{
		return Create(address, peerCount, channelCount, 0, 0);
	}

	bool ENetHost::Create(const IpAddress& address, std::size_t peerCount, std::size_t channelCount, UInt32 incomingBandwidth, UInt32 outgoingBandwidth)
	{
		NazaraAssert(address.IsValid(), "Invalid listening address");

		if (peerCount > ENetConstants::ENetProtocol_MaximumPeerId)
		{
			NazaraError("Peer count exceeds maximum peer count supported by protocol (" + String::Number(ENetConstants::ENetProtocol_MaximumPeerId) + ")");
			return false;
		}

		if (!InitSocket(address))
			return false;

		m_peers.resize(peerCount);

		m_address = address;
		m_randomSeed = *reinterpret_cast<UInt32*>(this);
		m_randomSeed += s_randomGenerator();
		m_randomSeed = (m_randomSeed << 16) | (m_randomSeed >> 16);
		m_channelLimit = Clamp<std::size_t>(channelCount, ENetConstants::ENetProtocol_MinimumChannelCount, ENetConstants::ENetProtocol_MaximumChannelCount);
		m_incomingBandwidth = incomingBandwidth;
		m_outgoingBandwidth = outgoingBandwidth;
		m_bandwidthThrottleEpoch = 0;
		m_recalculateBandwidthLimits = false;
		m_mtu = ENetConstants::ENetHost_DefaultMTU;
		m_commandCount = 0;
		m_bufferCount = 0;
		m_receivedAddress = IpAddress::AnyIpV4;
		m_receivedData = nullptr;
		m_receivedDataLength = 0;

		m_totalSentData = 0;
		m_totalSentPackets = 0;
		m_totalReceivedData = 0;
		m_totalReceivedPackets = 0;

		m_bandwidthLimitedPeers = 0;
		m_connectedPeers = 0;
		m_duplicatePeers = ENetConstants::ENetProtocol_MaximumPeerId;
		m_maximumPacketSize = ENetConstants::ENetHost_DefaultMaximumPacketSize;
		m_maximumWaitingData = ENetConstants::ENetHost_DefaultMaximumWaitingData;

		return true;
	}

	void ENetHost::Flush()
	{
		m_serviceTime = GetElapsedMilliseconds();

		SendOutgoingCommands(nullptr, 0);
	}

	int ENetHost::Service(ENetEvent* event, UInt32 timeout)
	{
		UInt32 waitCondition;

		if (event)
		{
			event->type = ENetEventType::None;
			event->peer = nullptr;
			event->packet = nullptr;

			if (DispatchIncomingCommands(event))
				return 1;
		}

		m_serviceTime = GetElapsedMilliseconds();
		timeout += m_serviceTime;

		do
		{
			if (ENET_TIME_DIFFERENCE(m_serviceTime, m_bandwidthThrottleEpoch) >= ENetConstants::ENetHost_BandwidthThrottleInterval)
				ThrottleBandwidth();

			switch (SendOutgoingCommands(event, true))
			{
				case 1:
					return 1;

				case -1:
	#ifdef ENET_DEBUG
					perror("Error sending outgoing packets");
	#endif
					return -1;

				default:
					break;
			}

			switch (ReceiveIncomingCommands(event))
			{
			case 1:
				return 1;

			case -1:
#ifdef ENET_DEBUG
				perror("Error receiving incoming packets");
#endif

				return -1;

			default:
				break;
			}

			switch (SendOutgoingCommands(event, 1))
			{
			case 1:
				return 1;

			case -1:
#ifdef ENET_DEBUG
				perror("Error sending outgoing packets");
#endif

				return -1;

			default:
				break;
			}

			if (event)
			{
				switch (DispatchIncomingCommands(event))
				{
					case 1:
						return 1;

					case -1:
	#ifdef ENET_DEBUG
						perror("Error dispatching incoming packets");
	#endif

						return -1;

					default:
						break;
				}
			}

			if (ENET_TIME_GREATER_EQUAL(m_serviceTime, timeout))
				return 0;

			for (;;)
			{
				m_serviceTime = GetElapsedMilliseconds();

				if (ENET_TIME_GREATER_EQUAL(m_serviceTime, timeout))
					return 0;

				SocketError error;
				if (m_poller.Wait(ENET_TIME_DIFFERENCE(timeout, m_serviceTime), &error))
					break;

				if (error != SocketError_NoError)
					return -1;
			}

			m_serviceTime = GetElapsedMilliseconds();
		}
		while (m_poller.IsReady(m_socket));

		return 0;
	}

	bool ENetHost::InitSocket(const IpAddress& address)
	{
		if (!m_socket.Create(address.GetProtocol()))
			return false;

		m_socket.EnableBlocking(false);
		m_socket.EnableBroadcasting(true);
		m_socket.SetReceiveBufferSize(ENetConstants::ENetHost_ReceiveBufferSize);
		m_socket.SetSendBufferSize(ENetConstants::ENetHost_SendBufferSize);

		if (!address.IsLoopback())
		{
			if (m_socket.Bind(address) != SocketState_Bound)
			{
				NazaraError("Failed to bind address " + address.ToString());
				return false;
			}
		}

		m_poller.RegisterSocket(m_socket);

		return true;
	}

	bool ENetHost::DispatchIncomingCommands(ENetEvent* event)
	{
		for (std::size_t bit = m_dispatchQueue.FindFirst(); bit != m_dispatchQueue.npos; bit = m_dispatchQueue.FindNext(bit))
		{
			m_dispatchQueue.Reset(bit);

			ENetPeer& peer = m_peers[bit];
			switch (peer.m_state)
			{
				case ENetPeerState::ConnectionPending:
				case ENetPeerState::ConnectionSucceeded:
					peer.ChangeState(ENetPeerState::Connected);

					event->type = ENetEventType::Connect;
					event->peer = &peer;
					event->data = peer.m_eventData;
					return true;

				case ENetPeerState::Zombie:
					m_recalculateBandwidthLimits = true;

					event->type = ENetEventType::Disconnect;
					event->peer = &peer;
					event->data = peer.m_eventData;

					peer.Reset();
					return true;

				case ENetPeerState::Connected:
					if (peer.m_dispatchedCommands.empty())
						continue;

					if (!peer.Receive(&event->packet, &event->channelId))
						continue;

					event->type = ENetEventType::Receive;
					event->peer = &peer;

					if (!peer.m_dispatchedCommands.empty())
						AddToDispatchQueue(&peer);

					return true;
			}
		}

		return false;
	}

	ENetPeer* ENetHost::HandleConnect(ENetProtocolHeader* header, ENetProtocol* command)
	{
		UInt32 channelCount = NetToHost(command->connect.channelCount);

		if (channelCount < ENetProtocol_MinimumChannelCount || channelCount > ENetProtocol_MaximumChannelCount)
			return nullptr;

		std::size_t duplicatePeers = 0;
		ENetPeer* peer = nullptr;
		for (ENetPeer& currentPeer : m_peers)
		{
			if (currentPeer.m_state == ENetPeerState::Disconnected)
			{
				if (!peer)
					peer = &currentPeer;
			}
			else if (currentPeer.m_state != ENetPeerState::Connecting)
			{
				// Compare users without comparing their port
				IpAddress first(currentPeer.m_address);
				first.SetPort(0);

				IpAddress second(m_receivedAddress);
				second.SetPort(0);

				if (first == second)
				{
					if (currentPeer.m_address.GetPort() == m_receivedAddress.GetPort() && currentPeer.m_connectID == command->connect.connectID)
						return nullptr;

					++duplicatePeers;
				}
			}
		}

		if (!peer || duplicatePeers >= m_duplicatePeers)
			return nullptr;

		channelCount = std::min(channelCount, m_channelLimit);

		peer->InitIncoming(channelCount, m_receivedAddress, command->connect);

		UInt32 windowSize;
		if (m_incomingBandwidth == 0)
			windowSize = ENetConstants::ENetProtocol_MaximumWindowSize;
		else
			windowSize = (m_incomingBandwidth / ENetConstants::ENetPeer_WindowSizeScale) * ENetConstants::ENetProtocol_MinimumWindowSize;

		windowSize = std::max(windowSize, NetToHost(command->connect.windowSize));
		windowSize = Clamp<UInt32>(windowSize, ENetConstants::ENetProtocol_MinimumWindowSize, ENetConstants::ENetProtocol_MaximumWindowSize);

		ENetProtocol verifyCommand;
		verifyCommand.header.command                           = ENetProtocolCommand_VerifyConnect | ENetProtocolFlag_Acknowledge;
		verifyCommand.header.channelID                         = 0xFF;
		verifyCommand.verifyConnect.outgoingPeerID             = HostToNet(peer->m_incomingPeerID);
		verifyCommand.verifyConnect.incomingSessionID          = peer->m_outgoingSessionID;
		verifyCommand.verifyConnect.outgoingSessionID          = peer->m_incomingSessionID;
		verifyCommand.verifyConnect.mtu                        = HostToNet(peer->m_mtu);
		verifyCommand.verifyConnect.windowSize                 = HostToNet(windowSize);
		verifyCommand.verifyConnect.channelCount               = HostToNet(channelCount);
		verifyCommand.verifyConnect.incomingBandwidth          = HostToNet(m_incomingBandwidth);
		verifyCommand.verifyConnect.outgoingBandwidth          = HostToNet(m_outgoingBandwidth);
		verifyCommand.verifyConnect.packetThrottleInterval     = HostToNet(peer->m_packetThrottleInterval);
		verifyCommand.verifyConnect.packetThrottleAcceleration = HostToNet(peer->m_packetThrottleAcceleration);
		verifyCommand.verifyConnect.packetThrottleDeceleration = HostToNet(peer->m_packetThrottleDeceleration);
		verifyCommand.verifyConnect.connectID = peer->m_connectID;

		peer->QueueOutgoingCommand(verifyCommand, nullptr, 0, 0);

		return peer;
	}

	bool ENetHost::HandleIncomingCommands(ENetEvent* event)
	{
		if (m_receivedDataLength < NazaraOffsetOf(ENetProtocolHeader, sentTime))
			return false;

		ENetProtocolHeader* header = reinterpret_cast<ENetProtocolHeader*>(m_receivedData);

		peerID = NetToHost(header->peerID);
		sessionID = (peerID & ENET_PROTOCOL_HEADER_SESSION_MASK) >> ENET_PROTOCOL_HEADER_SESSION_SHIFT;
		flags = peerID & ENET_PROTOCOL_HEADER_FLAG_MASK;
		peerID &= ~(ENET_PROTOCOL_HEADER_FLAG_MASK | ENET_PROTOCOL_HEADER_SESSION_MASK);

		headerSize = (flags & ENET_PROTOCOL_HEADER_FLAG_SENT_TIME ? sizeof(ENetProtocolHeader) : (size_t) & ((ENetProtocolHeader *)0)->sentTime);
		if (host->checksum != NULL)
			headerSize += sizeof(enet_uint32);

		if (peerID == ENET_PROTOCOL_MAXIMUM_PEER_ID)
			peer = NULL;
		else
			if (peerID >= host->peerCount)
				return 0;
			else
			{
				peer = &host->peers[peerID];

				if (peer->state == ENET_PEER_STATE_DISCONNECTED ||
					peer->state == ENET_PEER_STATE_ZOMBIE ||
					((host->receivedAddress.host != peer->address.host ||
						host->receivedAddress.port != peer->address.port) &&
						peer->address.host != ENET_HOST_BROADCAST) ||
						(peer->outgoingPeerID < ENET_PROTOCOL_MAXIMUM_PEER_ID &&
							sessionID != peer->incomingSessionID))
					return 0;
			}

		return false;
	}

	bool ENetHost::HandleSendReliable(ENetPeer& peer, const ENetProtocol& command, UInt8** currentData)
	{
		if (command.header.channelID >= peer.m_channels.size() || (peer.m_state != ENetPeerState::Connected && peer.m_state != ENetPeerState::DisconnectLater))
			return false;

		UInt16 dataLength = NetToHost(command.sendReliable.dataLength);
		*currentData += dataLength;
		if (dataLength >= m_maximumPacketSize || *currentData < m_receivedData || *currentData > &m_receivedData[m_receivedDataLength])
			return false;

		if (!peer.QueueIncomingCommand(command, co))

		return true;
	}

	int ENetHost::ReceiveIncomingCommands(ENetEvent* event)
	{
		for (unsigned int i = 0; i < 256; ++i)
		{
			NetPacket packet;
			
			std::size_t receivedLength;
			if (!m_socket.Receive(m_packetData[0].data(), m_packetData[0].size(), &m_receivedAddress, &receivedLength))
				return -1; //< Error

			if (receivedLength == 0)
				return 0;

			m_receivedData = m_packetData[0].data();
			m_receivedDataLength = receivedLength;

			m_totalReceivedData += receivedLength;
			m_totalReceivedPackets++;

			// Intercept

			switch (HandleIncomingCommands(event))
			{
				case 1:
					return 1;

				case -1:
					return -1;
			
				default:
					break;
			}
		}

		return -1;
	}

	void ENetHost::NotifyConnect(ENetPeer* peer, ENetEvent* event)
	{
		m_recalculateBandwidthLimits = true;

		if (event)
		{
			peer->ChangeState(ENetPeerState::Connected);

			event->type = ENetEventType::Connect;
			event->peer = peer;
			event->data = peer->m_eventData;
		}
		else
			peer->DispatchState(peer->m_state == ENetPeerState::Connecting ? ENetPeerState::ConnectionSucceeded : ENetPeerState::ConnectionPending);
	}

	void ENetHost::NotifyDisconnect(ENetPeer* peer, ENetEvent* event)
	{
		if (peer->m_state >= ENetPeerState::ConnectionPending)
			m_recalculateBandwidthLimits = true;

		if (peer->m_state != ENetPeerState::Connecting && (peer->m_state < ENetPeerState::ConnectionSucceeded))
			peer->Reset();
		else if (event)
		{
			event->type = ENetEventType::Disconnect;
			event->peer = peer;
			event->data = peer->m_eventData;

			peer->Reset();
		}
		else
		{
			peer->m_eventData = 0;

			peer->DispatchState(ENetPeerState::Zombie);
		}
	}

	void ENetHost::ThrottleBandwidth()
	{
		UInt32 currentTime = GetElapsedMilliseconds();
		UInt32 elapsedTime = currentTime - m_bandwidthThrottleEpoch;

		if (elapsedTime < ENetConstants::ENetHost_BandwidthThrottleInterval)
			return;

		m_bandwidthThrottleEpoch = currentTime;

		if (m_connectedPeers == 0)
			return;

		UInt32 dataTotal = ~0;
		UInt32 bandwidth = ~0;

		if (m_outgoingBandwidth != 0)
		{
			bandwidth = (m_outgoingBandwidth * elapsedTime) / 1000;

			dataTotal = 0;
			for (ENetPeer& peer : m_peers)
			{
				if (peer.m_state != ENetPeerState::Connected && peer.m_state != ENetPeerState::DisconnectLater)
					continue;

				dataTotal += peer.m_outgoingDataTotal;
			}
		}

		UInt32 peersRemaining = m_connectedPeers;
		UInt32 bandwidthLimit = ~0;
		UInt32 throttle = ~0;
		bool needsAdjustment = m_bandwidthLimitedPeers > 0;

		while (peersRemaining > 0 && needsAdjustment)
		{
			needsAdjustment = false;

			if (dataTotal <= bandwidth)
				throttle = ENetConstants::ENetPeer_PacketThrottleScale;
			else
				throttle = (bandwidth * ENetConstants::ENetPeer_PacketThrottleScale) / dataTotal;

			for (ENetPeer& peer : m_peers)
			{
				if ((peer.m_state != ENetPeerState::Connected && peer.m_state != ENetPeerState::DisconnectLater) ||
					peer.m_incomingBandwidth == 0 || peer.m_outgoingBandwidthThrottleEpoch == currentTime)
					continue;

				UInt32 peerBandwidth = (peer.m_incomingBandwidth * elapsedTime) / 1000;
				if ((throttle * peer.m_outgoingDataTotal) / ENetConstants::ENetPeer_PacketThrottleScale <= peerBandwidth)
					continue;

				peer.m_packetThrottleLimit = (peerBandwidth * ENetConstants::ENetPeer_PacketThrottleScale) / peer.m_outgoingDataTotal;

				if (peer.m_packetThrottleLimit == 0)
					peer.m_packetThrottleLimit = 1;

				if (peer.m_packetThrottle > peer.m_packetThrottleLimit)
					peer.m_packetThrottle = peer.m_packetThrottleLimit;

				peer.m_outgoingBandwidthThrottleEpoch = currentTime;

				peer.m_incomingDataTotal = 0;
				peer.m_outgoingDataTotal = 0;

				needsAdjustment = true;
				--peersRemaining;
				bandwidth -= peerBandwidth;
				dataTotal -= peerBandwidth;
			}
		}

		if (peersRemaining > 0)
		{
			if (dataTotal <= bandwidth)
				throttle = ENetConstants::ENetPeer_PacketThrottleScale;
			else
				throttle = (bandwidth * ENetConstants::ENetPeer_PacketThrottleScale) / dataTotal;

			for (ENetPeer& peer : m_peers)
			{
				if ((peer.m_state != ENetPeerState::Connected && peer.m_state != ENetPeerState::DisconnectLater) ||
					peer.m_outgoingBandwidthThrottleEpoch == currentTime)
					continue;

				peer.m_packetThrottleLimit = throttle;

				if (peer.m_packetThrottle > peer.m_packetThrottleLimit)
					peer.m_packetThrottle = peer.m_packetThrottleLimit;

				peer.m_incomingDataTotal = 0;
				peer.m_outgoingDataTotal = 0;
			}
		}

		if (m_recalculateBandwidthLimits)
		{
			m_recalculateBandwidthLimits = false;

			peersRemaining = m_connectedPeers;
			bandwidth = m_incomingBandwidth;
			needsAdjustment = true;

			if (bandwidth == 0)
				bandwidthLimit = 0;
			else
			{
				while (peersRemaining > 0 && needsAdjustment)
				{
					needsAdjustment = false;
					bandwidthLimit = bandwidth / peersRemaining;

					for (ENetPeer& peer : m_peers)
					{
						if ((peer.m_state != ENetPeerState::Connected && peer.m_state != ENetPeerState::DisconnectLater) ||
							peer.m_incomingBandwidthThrottleEpoch == currentTime)
							continue;

						if (peer.m_outgoingBandwidth > 0 && peer.m_outgoingBandwidth >= bandwidthLimit)
							continue;

						peer.m_incomingBandwidthThrottleEpoch = currentTime;

						needsAdjustment = true;
						--peersRemaining;
						bandwidth -= peer.m_outgoingBandwidth;
					}
				}
			}

			for (ENetPeer& peer : m_peers)
			{
				if (peer.m_state != ENetPeerState::Connected && peer.m_state != ENetPeerState::DisconnectLater)
					continue;

				ENetProtocol command;
				command.header.command = ENetProtocolCommand_BandwidthLimit | ENetProtocolFlag_Acknowledge;
				command.header.channelID = 0xFF;
				command.bandwidthLimit.outgoingBandwidth = HostToNet(m_outgoingBandwidth);

				if (peer.m_incomingBandwidthThrottleEpoch == currentTime)
					command.bandwidthLimit.incomingBandwidth = HostToNet(peer.m_outgoingBandwidth);
				else
					command.bandwidthLimit.incomingBandwidth = HostToNet(bandwidthLimit);

				peer.QueueOutgoingCommand(command, nullptr, 0, 0);
			}
		}
	}

	bool ENetHost::Initialize()
	{
		std::random_device device;
		s_randomGenerator.seed(device());
		s_randomGenerator64.seed(device());

		return true;
	}

	void ENetHost::Uninitialize()
	{
	}

	std::mt19937 ENetHost::s_randomGenerator;
	std::mt19937_64 ENetHost::s_randomGenerator64;
}
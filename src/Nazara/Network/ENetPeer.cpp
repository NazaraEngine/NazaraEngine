#include <Nazara/Network/ENetPeer.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Network/ENetHost.hpp>
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

	ENetPeer::ENetPeer(ENetHost* host, UInt16 peerId) :
	m_host(host),
	m_packetPool(sizeof(ENetPacket)),
	m_incomingSessionID(0xFF),
	m_outgoingSessionID(0xFF),
	m_incomingPeerID(peerId)
	{
		Reset();
	}

	void ENetPeer::Disconnect(UInt32 data)
	{
		if (m_state == ENetPeerState::Disconnecting ||
			m_state == ENetPeerState::Disconnected ||
			m_state == ENetPeerState::AcknowledgingDisconnect ||
			m_state == ENetPeerState::Zombie)
			return;

		ResetQueues();

		ENetProtocol command(ENetProtocolCommand_Disconnect, 0xFF);
		command.disconnect.data = HostToNet(data);

		if (IsConnected())
			command.header.command |= ENetProtocolFlag_Acknowledge;
		else
			command.header.command |= ENetProtocolFlag_Unsequenced;

		QueueOutgoingCommand(command);

		if (IsConnected())
		{
			OnDisconnect();

			m_state = ENetPeerState::Disconnecting;
		}
		else
		{
			m_host->Flush();

			Reset();
		}
	}

	void ENetPeer::DisconnectLater(UInt32 data)
	{
		if (IsConnected() && HasPendingCommands())
		{
			m_state = ENetPeerState::DisconnectLater;
			m_eventData = data;
		}
		else
			Disconnect(data);
	}

	void ENetPeer::DisconnectNow(UInt32 data)
	{
		if (m_state == ENetPeerState::Disconnected)
			return;

		if (m_state != ENetPeerState::Zombie && m_state != ENetPeerState::Disconnecting)
		{
			ResetQueues();

			ENetProtocol command(ENetProtocolCommand_Disconnect | ENetProtocolFlag_Unsequenced, 0xFF);
			command.disconnect.data = HostToNet(data);
			QueueOutgoingCommand(command);

			m_host->Flush();
		}

		Reset();
	}

	void ENetPeer::Ping()
	{
		if (m_state != ENetPeerState::Connected)
			return;

		ENetProtocol command;
		command.header.command = ENetProtocolCommand_Ping | ENetProtocolFlag_Acknowledge;
		command.header.channelID = 0xFF;
		QueueOutgoingCommand(command);
	}

	bool ENetPeer::Receive(ENetPacketRef* packet, UInt8* channelId)
	{
		if (m_dispatchedCommands.empty())
			return false;

		IncomingCommmand& incomingCommand = m_dispatchedCommands.front();

		m_totalWaitingData -= incomingCommand.packet->data.GetDataSize();

		if (packet)
			*packet = std::move(incomingCommand.packet);

		if (channelId)
			*channelId = incomingCommand.command.header.channelID;

		m_dispatchedCommands.pop_front();

		return true;
	}

	void ENetPeer::Reset()
	{
		OnDisconnect();

		m_outgoingPeerID = ENetConstants::ENetProtocol_MaximumPeerId;
		m_connectID = 0;

		m_state = ENetPeerState::Disconnected;

		m_incomingBandwidth = 0;
		m_outgoingBandwidth = 0;
		m_incomingBandwidthThrottleEpoch = 0;
		m_outgoingBandwidthThrottleEpoch = 0;
		m_incomingDataTotal = 0;
		m_outgoingDataTotal = 0;
		m_lastSendTime = 0;
		m_lastReceiveTime = 0;
		m_nextTimeout = 0;
		m_earliestTimeout = 0;
		m_packetLossEpoch = 0;
		m_packetsSent = 0;
		m_packetsLost = 0;
		m_packetLoss = 0;
		m_packetLossVariance = 0;
		m_packetThrottle = ENetConstants::ENetProtocol_MaximumWindowSize;
		m_packetThrottleLimit = ENetConstants::ENetPeer_PacketThrottleScale;
		m_packetThrottleCounter = 0;
		m_packetThrottleEpoch = 0;
		m_packetThrottleAcceleration = ENetConstants::ENetPeer_PacketThrottleAcceleration;
		m_packetThrottleDeceleration = ENetConstants::ENetPeer_PacketThrottleDeceleration;
		m_packetThrottleInterval = ENetConstants::ENetPeer_PacketThrottleInterval;
		m_pingInterval = ENetConstants::ENetPeer_PingInterval;
		m_timeoutLimit = ENetConstants::ENetPeer_TimeoutLimit;
		m_timeoutMinimum = ENetConstants::ENetPeer_TimeoutMinimum;
		m_timeoutMaximum = ENetConstants::ENetPeer_TimeoutMaximum;
		m_lastRoundTripTime = ENetConstants::ENetPeer_DefaultRoundTripTime;
		m_lowestRoundTripTime = ENetConstants::ENetPeer_DefaultRoundTripTime;
		m_lastRoundTripTimeVariance = 0;
		m_highestRoundTripTimeVariance = 0;
		m_roundTripTime = ENetConstants::ENetPeer_DefaultRoundTripTime;
		m_roundTripTimeVariance = 0;
		m_mtu = m_host->m_mtu;
		m_reliableDataInTransit = 0;
		m_outgoingReliableSequenceNumber = 0;
		m_windowSize = ENetConstants::ENetProtocol_MaximumWindowSize;
		m_incomingUnsequencedGroup = 0;
		m_outgoingUnsequencedGroup = 0;
		m_eventData = 0;
		m_totalPacketLost = 0;
		m_totalPacketSent = 0;
		m_totalWaitingData = 0;

		m_unsequencedWindow.fill(0);

		ResetQueues();
	}

	bool ENetPeer::Send(UInt8 channelId, ENetPacketFlags flags, NetPacket&& packet)
	{
		ENetPacket* enetPacket = m_packetPool.New<ENetPacket>();
		enetPacket->flags = flags;
		enetPacket->data = std::move(packet);
		enetPacket->owner = &m_packetPool;

		return Send(channelId, enetPacket);
	}

	bool ENetPeer::Send(UInt8 channelId, ENetPacketRef packetRef)
	{
		if (m_state != ENetPeerState::Connected || channelId >= m_channels.size() || packetRef->data.GetDataSize() > m_host->m_maximumPacketSize)
			return false;

		Channel& channel = m_channels[channelId];

		std::size_t fragmentLength = m_mtu - sizeof(ENetProtocolHeader) - sizeof(ENetProtocolSendFragment);
		//if (m_host->m_checksum != nullptr)
		//	fragmentLength -= sizeof(UInt32);

		UInt32 packetSize = static_cast<UInt32>(packetRef->data.GetDataSize());
		if (packetSize > fragmentLength)
		{
			UInt32 fragmentCount = (packetSize + fragmentLength - 1) / fragmentLength;
			UInt32 fragmentNumber;
			UInt32 fragmentOffset;

			UInt8 commandNumber;
			UInt16 startSequenceNumber;

			if (fragmentCount > ENetConstants::ENetProtocol_MaximumFragmentCount)
				return false;

			if ((packetRef->flags & (ENetPacketFlag_Reliable | ENetPacketFlag_UnreliableFragment)) == ENetPacketFlag_UnreliableFragment &&
				channel.outgoingUnreliableSequenceNumber < 0xFFFF)
			{
				commandNumber = ENetProtocolCommand_SendUnreliable;
				startSequenceNumber = HostToNet(channel.outgoingUnreliableSequenceNumber + 1);
			}
			else
			{
				commandNumber = ENetProtocolCommand_SendFragment | ENetProtocolFlag_Acknowledge;
				startSequenceNumber = HostToNet(channel.outgoingReliableSequenceNumber + 1);
			}

			for (fragmentNumber = 0,
				fragmentOffset = 0;
				fragmentOffset < packetSize;
				++fragmentNumber,
				fragmentOffset += fragmentLength)
			{
				if (packetSize - fragmentOffset < fragmentLength)
					fragmentLength = packetSize - fragmentOffset;

				OutgoingCommand outgoingCommand;
				outgoingCommand.fragmentOffset = fragmentOffset;
				outgoingCommand.fragmentLength = static_cast<UInt16>(fragmentLength);
				outgoingCommand.packet = packetRef;
				outgoingCommand.command.header.command = commandNumber;
				outgoingCommand.command.header.channelID = channelId;
				outgoingCommand.command.sendFragment.startSequenceNumber = startSequenceNumber;
				outgoingCommand.command.sendFragment.dataLength = HostToNet(fragmentLength);
				outgoingCommand.command.sendFragment.fragmentCount = HostToNet(fragmentCount);
				outgoingCommand.command.sendFragment.fragmentNumber = HostToNet(fragmentNumber);
				outgoingCommand.command.sendFragment.totalLength = HostToNet(packetSize);
				outgoingCommand.command.sendFragment.fragmentOffset = HostToNet(fragmentOffset);

				SetupOutgoingCommand(outgoingCommand);
			}

			return true;
		}

		ENetProtocol command;
		command.header.channelID = channelId;

		if ((packetRef->flags & (ENetPacketFlag_Reliable | ENetPacketFlag_Unsequenced)) == ENetPacketFlag_Unsequenced)
		{
			command.header.command = ENetProtocolCommand_SendUnsequenced | ENetProtocolFlag_Unsequenced;
			command.sendUnsequenced.dataLength = HostToNet(UInt16(packetRef->data.GetDataSize()));
		}
		else if (packetRef->flags & ENetPacketFlag_Reliable || channel.outgoingUnreliableSequenceNumber >= 0xFFFF)
		{
			command.header.command = ENetProtocolCommand_SendReliable | ENetProtocolFlag_Acknowledge;
			command.sendReliable.dataLength = HostToNet(UInt16(packetRef->data.GetDataSize()));
		}
		else
		{
			command.header.command = ENetProtocolCommand_SendUnreliable;
			command.sendUnreliable.dataLength = HostToNet(UInt16(packetRef->data.GetDataSize()));
		}

		QueueOutgoingCommand(command, packetRef, 0, packetSize);

		return true;
	}

	void ENetPeer::ThrottleConfigure(UInt32 interval, UInt32 acceleration, UInt32 deceleration)
	{
		m_packetThrottleInterval = interval;
		m_packetThrottleAcceleration = acceleration;
		m_packetThrottleDeceleration = deceleration;

		ENetProtocol command(ENetProtocolCommand_ThrottleConfigure | ENetProtocolFlag_Acknowledge, 0xFF);
		command.throttleConfigure.packetThrottleInterval = HostToNet(interval);
		command.throttleConfigure.packetThrottleAcceleration = HostToNet(acceleration);
		command.throttleConfigure.packetThrottleDeceleration = HostToNet(deceleration);
		QueueOutgoingCommand(command);
	}

	bool ENetPeer::CheckTimeouts(ENetEvent* event)
	{
		auto currentCommand = m_sentReliableCommands.begin();
		while (currentCommand != m_sentReliableCommands.end())
		{
			auto outgoingCommand = currentCommand;

			++currentCommand;

			if (ENET_TIME_DIFFERENCE(m_host->m_serviceTime, outgoingCommand->sentTime) < outgoingCommand->roundTripTimeout)
				continue;

			if (m_earliestTimeout == 0 || ENET_TIME_LESS(outgoingCommand->sentTime, m_earliestTimeout))
				m_earliestTimeout = outgoingCommand->sentTime;

			if (m_earliestTimeout != 0 && (ENET_TIME_DIFFERENCE(m_host->m_serviceTime, m_earliestTimeout) >= m_timeoutMaximum ||
				(outgoingCommand->roundTripTimeout >= outgoingCommand->roundTripTimeoutLimit && ENET_TIME_DIFFERENCE(m_host->m_serviceTime, m_earliestTimeout) >= m_timeoutMinimum)))
			{
				m_host->NotifyDisconnect(this, event);
				return true;
			}

			if (outgoingCommand->packet)
				m_reliableDataInTransit -= outgoingCommand->fragmentLength;

			++m_packetsLost;
			++m_totalPacketLost;

			// http://lists.cubik.org/pipermail/enet-discuss/2014-May/002308.html
			outgoingCommand->roundTripTimeout = m_roundTripTime + 4 * m_roundTripTimeVariance;
			outgoingCommand->roundTripTimeoutLimit = m_timeoutLimit * outgoingCommand->roundTripTimeout;

			m_outgoingReliableCommands.emplace_front(std::move(*outgoingCommand));
			m_sentReliableCommands.erase(outgoingCommand);

			// Okay this should just never procs, I don't see how it would be possible
			/*if (currentCommand == enet_list_begin(&peer->sentReliableCommands) &&
			!enet_list_empty(&peer->sentReliableCommands))
			{
			outgoingCommand = (ENetOutgoingCommand *) currentCommand;

			peer->nextTimeout = outgoingCommand->sentTime + outgoingCommand->roundTripTimeout;
			}*/
		}

		return false;
	}

	void ENetPeer::DispatchState(ENetPeerState state)
	{
		ChangeState(state);

		m_host->AddToDispatchQueue(this);
	}

	void ENetPeer::DispatchIncomingReliableCommands(Channel& channel)
	{
		auto currentCommand = channel.incomingReliableCommands.begin();
		for (; currentCommand != channel.incomingReliableCommands.end(); ++currentCommand)
		{
			IncomingCommmand& incomingCommand = *currentCommand;

			if (incomingCommand.fragmentsRemaining > 0 || incomingCommand.reliableSequenceNumber != (channel.incomingReliableSequenceNumber + 1))
				break;

			channel.incomingReliableSequenceNumber = incomingCommand.reliableSequenceNumber;

			if (incomingCommand.fragments.GetSize() > 0)
				channel.incomingReliableSequenceNumber += incomingCommand.fragments.GetSize() - 1;
		}

		if (currentCommand == channel.incomingReliableCommands.begin())
			return;

		channel.incomingUnreliableSequenceNumber = 0;

		m_dispatchedCommands.splice(m_dispatchedCommands.end(), channel.incomingReliableCommands, channel.incomingReliableCommands.begin(), currentCommand);

		m_host->AddToDispatchQueue(this);

		if (!channel.incomingUnreliableCommands.empty())
			DispatchIncomingUnreliableCommands(channel);
	}

	void ENetPeer::DispatchIncomingUnreliableCommands(Channel& channel)
	{
		std::list<IncomingCommmand>::iterator currentCommand;
		std::list<IncomingCommmand>::iterator droppedCommand;
		std::list<IncomingCommmand>::iterator startCommand;

		for (droppedCommand = startCommand = currentCommand = channel.incomingUnreliableCommands.begin();
		     currentCommand != channel.incomingUnreliableCommands.end();
		     ++currentCommand)
		{
			IncomingCommmand& incomingCommand = *currentCommand;

			if ((incomingCommand.command.header.command & ENetProtocolCommand_Mask) == ENetProtocolCommand_SendUnsequenced)
				continue;

			if (incomingCommand.reliableSequenceNumber == channel.incomingReliableSequenceNumber)
			{
				if (incomingCommand.fragmentsRemaining <= 0)
				{
					channel.incomingUnreliableSequenceNumber = incomingCommand.unreliableSequenceNumber;
					continue;
				}

				if (startCommand != currentCommand)
				{
					m_dispatchedCommands.splice(m_dispatchedCommands.end(), channel.incomingUnreliableCommands, startCommand, currentCommand);

					m_host->AddToDispatchQueue(this);

					droppedCommand = currentCommand;
				}
				else if (droppedCommand != currentCommand)
				{
					droppedCommand = currentCommand;
					--droppedCommand;
				}
			}
			else
			{
				UInt16 reliableWindow = incomingCommand.reliableSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;
				UInt16 currentWindow  = channel.incomingReliableSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;

				if (incomingCommand.reliableSequenceNumber < channel.incomingReliableSequenceNumber)
					reliableWindow += ENetConstants::ENetPeer_ReliableWindows;

				if (reliableWindow >= currentWindow && reliableWindow < currentWindow + ENetConstants::ENetPeer_ReliableWindowSize - 1)
					break;

				droppedCommand = currentCommand;
				++droppedCommand;

				if (startCommand != currentCommand)
				{
					m_dispatchedCommands.splice(m_dispatchedCommands.end(), channel.incomingUnreliableCommands, startCommand, currentCommand);

					m_host->AddToDispatchQueue(this);
				}
			}

			startCommand = currentCommand;
			++startCommand;
		}

		if (startCommand != currentCommand)
		{
			m_dispatchedCommands.splice(m_dispatchedCommands.end(), channel.incomingUnreliableCommands, startCommand, currentCommand);

			m_host->AddToDispatchQueue(this);

			droppedCommand = currentCommand;
		}

		channel.incomingUnreliableCommands.erase(channel.incomingUnreliableCommands.begin(), droppedCommand);
	}

	bool ENetPeer::HandleAcknowledge(const ENetProtocol* command, ENetEvent* event)
	{
		if (m_state == ENetPeerState::Disconnected || m_state == ENetPeerState::Zombie)
			return true;

		UInt32 serviceTime = m_host->GetServiceTime();

		UInt32 receivedSentTime = NetToHost(command->acknowledge.receivedSentTime);
		receivedSentTime |= serviceTime & 0xFFFF0000;
		if ((receivedSentTime & 0x8000) > (serviceTime & 0x8000))
			receivedSentTime -= 0x10000;

		if (ENET_TIME_LESS(serviceTime, receivedSentTime))
			return true;

		m_lastReceiveTime = serviceTime;
		m_earliestTimeout = 0;

		UInt32 roundTripTime = ENET_TIME_DIFFERENCE(serviceTime, receivedSentTime);

		Throttle(roundTripTime);

		m_roundTripTimeVariance -= m_roundTripTimeVariance / 4;

		if (roundTripTime >= m_roundTripTime)
		{
			m_roundTripTime += (roundTripTime - m_roundTripTime) / 8;
			m_roundTripTimeVariance += (roundTripTime - m_roundTripTime) / 4;
		}
		else
		{
			m_roundTripTime -= (m_roundTripTime - roundTripTime) / 8;
			m_roundTripTimeVariance += (m_roundTripTime - roundTripTime) / 4;
		}

		m_lowestRoundTripTime = std::min(m_lowestRoundTripTime, m_roundTripTime);
		m_highestRoundTripTimeVariance = std::max(m_highestRoundTripTimeVariance, m_roundTripTimeVariance);

		if (m_packetThrottleEpoch == 0 || ENET_TIME_DIFFERENCE(serviceTime, m_packetThrottleEpoch) >= m_packetThrottleInterval)
		{
			m_lastRoundTripTime = m_lowestRoundTripTime;
			m_lastRoundTripTimeVariance = m_highestRoundTripTimeVariance;
			m_lowestRoundTripTime = m_roundTripTime;
			m_highestRoundTripTimeVariance = m_roundTripTimeVariance;
			m_packetThrottleEpoch = serviceTime;
		}

		UInt16 receivedReliableSequenceNumber = NetToHost(command->acknowledge.receivedReliableSequenceNumber);

		ENetProtocolCommand commandNumber = RemoveSentReliableCommand(receivedReliableSequenceNumber, command->header.channelID);

		switch (m_state)
		{
			case ENetPeerState::AcknowledgingConnect:
				if (commandNumber != ENetProtocolCommand_VerifyConnect)
					return false;

				m_host->NotifyConnect(this, event);
				break;

			case ENetPeerState::Disconnecting:
				if (commandNumber != ENetProtocolCommand_Disconnect)
					return false;

				m_host->NotifyDisconnect(this, event);
				break;

			case ENetPeerState::DisconnectLater:
				if (!HasPendingCommands())
					Disconnect(m_eventData);

				break;

			default:
				break;
		}

		return true;
	}

	bool ENetPeer::HandleBandwidthLimit(const ENetProtocol* command)
	{
		if (!IsConnected())
			return false;

		if (m_incomingBandwidth != 0)
			--m_host->m_bandwidthLimitedPeers;

		m_incomingBandwidth = NetToHost(command->bandwidthLimit.incomingBandwidth);
		m_outgoingBandwidth = NetToHost(command->bandwidthLimit.outgoingBandwidth);

		if (m_incomingBandwidth != 0)
			++m_host->m_bandwidthLimitedPeers;

		if (m_incomingBandwidth == 0 && m_host->m_outgoingBandwidth == 0)
			m_windowSize = ENetConstants::ENetProtocol_MaximumWindowSize;
		else
		{
			if (m_incomingBandwidth == 0 || m_host->m_outgoingBandwidth == 0)
				m_windowSize = (std::max(m_incomingBandwidth, m_host->m_outgoingBandwidth) / ENetConstants::ENetPeer_WindowSizeScale) * ENetConstants::ENetProtocol_MinimumWindowSize;
			else
				m_windowSize = (std::min(m_incomingBandwidth, m_host->m_outgoingBandwidth) / ENetConstants::ENetPeer_WindowSizeScale) * ENetConstants::ENetProtocol_MinimumWindowSize;

			m_windowSize = Clamp<UInt32>(m_windowSize, ENetConstants::ENetProtocol_MinimumWindowSize, ENetConstants::ENetProtocol_MaximumWindowSize);
		}

		return true;
	}

	bool ENetPeer::HandleDisconnect(const ENetProtocol* command)
	{
		if (m_state == ENetPeerState::Disconnected || m_state == ENetPeerState::Zombie || m_state == ENetPeerState::AcknowledgingDisconnect)
			return true;

		ResetQueues();

		if (m_state == ENetPeerState::ConnectionSucceeded || m_state == ENetPeerState::Disconnecting || m_state == ENetPeerState::Connecting)
			DispatchState(ENetPeerState::Zombie);
		else
		{
			if (!IsConnected())
			{
				if (m_state == ENetPeerState::ConnectionPending)
					m_host->m_recalculateBandwidthLimits = true;

				Reset();
			}
			else
			{
				if (command->header.command & ENetProtocolFlag_Acknowledge)
					ChangeState(ENetPeerState::AcknowledgingDisconnect);
				else
					DispatchState(ENetPeerState::Zombie);
			}
		}

		if (m_state != ENetPeerState::Disconnected)
			m_eventData = NetToHost(command->disconnect.data);

		return true;
	}

	bool ENetPeer::HandlePing(const ENetProtocol* /*command*/)
	{
		if (!IsConnected())
			return false;

		return true;
	}

	bool ENetPeer::HandleSendFragment(const ENetProtocol* command, UInt8** data)
	{
		if (command->header.channelID >= m_channels.size() || !IsConnected())
			return false;

		UInt16 fragmentLength = NetToHost(command->sendFragment.dataLength);
		*data += fragmentLength;
		if (fragmentLength >= m_host->m_maximumPacketSize || *data < m_host->m_receivedData || *data > &m_host->m_receivedData[m_host->m_receivedDataLength])
			return false;

		ENetPeer::Channel& channel = m_channels[command->header.channelID];
		UInt32 startSequenceNumber = NetToHost(command->sendFragment.startSequenceNumber);
		UInt16 startWindow = startSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;
		UInt16 currentWindow = channel.incomingReliableSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;

		if (startSequenceNumber < channel.incomingReliableSequenceNumber)
			startWindow += ENetConstants::ENetPeer_ReliableWindows;

		if (startWindow < currentWindow || startWindow >= currentWindow + ENetConstants::ENetPeer_FreeReliableWindows - 1)
			return true;

		UInt32 fragmentNumber = NetToHost(command->sendFragment.fragmentNumber);
		UInt32 fragmentCount = NetToHost(command->sendFragment.fragmentCount);
		UInt32 fragmentOffset = NetToHost(command->sendFragment.fragmentOffset);
		UInt32 totalLength = NetToHost(command->sendFragment.totalLength);

		if (fragmentCount > ENetConstants::ENetProtocol_MaximumFragmentCount || fragmentNumber >= fragmentCount || totalLength > m_host->m_maximumPacketSize ||
			fragmentOffset >= totalLength || fragmentLength > totalLength - fragmentOffset)
			return false;

		ENetPeer::IncomingCommmand* startCommand = nullptr;
		for (auto currentCommand = channel.incomingReliableCommands.rbegin(); currentCommand != channel.incomingReliableCommands.rend(); ++currentCommand)
		{
			ENetPeer::IncomingCommmand& incomingCommand = *currentCommand;

			if (startSequenceNumber >= channel.incomingReliableSequenceNumber)
			{
				if (incomingCommand.reliableSequenceNumber < channel.incomingReliableSequenceNumber)
					continue;
			}
			else if (incomingCommand.reliableSequenceNumber >= channel.incomingReliableSequenceNumber)
				break;

			if (incomingCommand.reliableSequenceNumber <= startSequenceNumber)
			{
				if (incomingCommand.reliableSequenceNumber < startSequenceNumber)
					break;

				if ((incomingCommand.command.header.command & ENetProtocolCommand_Mask) != ENetProtocolCommand_SendFragment ||
					totalLength != incomingCommand.packet->data.GetDataSize() || fragmentCount != incomingCommand.fragments.GetSize())
					return false;

				startCommand = &incomingCommand;
				break;
			}
		}

		if (startCommand)
		{
			ENetProtocol hostCommand = *command;
			hostCommand.header.reliableSequenceNumber = startSequenceNumber;

			if (!QueueIncomingCommand(hostCommand, nullptr, totalLength, ENetPacketFlag_Reliable, fragmentCount))
				return false;
		}

		if (!startCommand->fragments.Test(fragmentNumber))
		{
			--startCommand->fragmentsRemaining;

			startCommand->fragments.Set(fragmentNumber, true);

			if (fragmentOffset + fragmentLength > startCommand->packet->data.GetDataSize())
				fragmentLength = startCommand->packet->data.GetDataSize() - fragmentOffset;

			std::memcpy(startCommand->packet->data.GetData() + NetPacket::HeaderSize + fragmentOffset, reinterpret_cast<const UInt8*>(command) + sizeof(ENetProtocolSendFragment), fragmentLength);

			if (startCommand->fragmentsRemaining <= 0)
				DispatchIncomingReliableCommands(channel);
		}

		return false;
	}

	bool ENetPeer::HandleSendReliable(const ENetProtocol* command, UInt8** data)
	{
		if (command->header.channelID >= m_channels.size() || !IsConnected())
			return false;

		UInt16 dataLength = NetToHost(command->sendReliable.dataLength);
		*data += dataLength;
		if (dataLength >= m_host->m_maximumPacketSize || *data < m_host->m_receivedData || *data > &m_host->m_receivedData[m_host->m_receivedDataLength])
			return false;

		if (!QueueIncomingCommand(*command, reinterpret_cast<const UInt8*>(command) + sizeof(ENetProtocolSendReliable), dataLength, ENetPacketFlag_Reliable, 0))
			return false;

		return true;
	}

	bool ENetPeer::HandleSendUnreliable(const ENetProtocol* command, UInt8** data)
	{
		if (command->header.channelID >= m_channels.size() || !IsConnected())
			return false;

		UInt16 dataLength = NetToHost(command->sendUnreliable.dataLength);
		*data += dataLength;
		if (dataLength >= m_host->m_maximumPacketSize || *data < m_host->m_receivedData || *data > &m_host->m_receivedData[m_host->m_receivedDataLength])
			return false;

		if (!QueueIncomingCommand(*command, reinterpret_cast<const UInt8*>(command) + sizeof(ENetProtocolSendUnreliable), dataLength, 0, 0))
			return false;

		return true;
	}

	bool ENetPeer::HandleSendUnreliableFragment(const ENetProtocol* command, UInt8** data)
	{
		if (command->header.channelID >= m_channels.size() || !IsConnected())
			return false;

		UInt16 fragmentLength = NetToHost(command->sendFragment.dataLength);
		*data += fragmentLength;
		if (fragmentLength >= m_host->m_maximumPacketSize || *data < m_host->m_receivedData || *data > &m_host->m_receivedData[m_host->m_receivedDataLength])
			return false;

		ENetPeer::Channel& channel = m_channels[command->header.channelID];
		UInt32 reliableSequenceNumber = command->header.reliableSequenceNumber;
		UInt32 startSequenceNumber = NetToHost(command->sendFragment.startSequenceNumber);

		UInt16 reliableWindow = reliableSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;
		UInt16 currentWindow = channel.incomingReliableSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;

		if (startSequenceNumber < channel.incomingReliableSequenceNumber)
			reliableWindow += ENetConstants::ENetPeer_ReliableWindows;

		if (reliableWindow < currentWindow || reliableWindow >= currentWindow + ENetConstants::ENetPeer_FreeReliableWindows - 1)
			return true;

		if (reliableSequenceNumber == channel.incomingReliableSequenceNumber && startSequenceNumber <= channel.incomingUnreliableSequenceNumber)
			return true;

		UInt32 fragmentNumber = NetToHost(command->sendFragment.fragmentNumber);
		UInt32 fragmentCount = NetToHost(command->sendFragment.fragmentCount);
		UInt32 fragmentOffset = NetToHost(command->sendFragment.fragmentOffset);
		UInt32 totalLength = NetToHost(command->sendFragment.totalLength);

		if (fragmentCount > ENetConstants::ENetProtocol_MaximumFragmentCount || fragmentNumber >= fragmentCount || totalLength > m_host->m_maximumPacketSize ||
		    fragmentOffset >= totalLength || fragmentLength > totalLength - fragmentOffset)
			return false;

		ENetPeer::IncomingCommmand* startCommand = nullptr;
		for (auto currentCommand = channel.incomingUnreliableCommands.rbegin(); currentCommand != channel.incomingUnreliableCommands.rend(); ++currentCommand)
		{
			ENetPeer::IncomingCommmand& incomingCommand = *currentCommand;

			if (startSequenceNumber >= channel.incomingReliableSequenceNumber)
			{
				if (incomingCommand.reliableSequenceNumber < channel.incomingReliableSequenceNumber)
					continue;
			}
			else if (incomingCommand.reliableSequenceNumber >= channel.incomingReliableSequenceNumber)
				break;

			if (incomingCommand.reliableSequenceNumber < reliableSequenceNumber)
				break;

			if (incomingCommand.reliableSequenceNumber > reliableSequenceNumber)
				continue;

			if (incomingCommand.unreliableSequenceNumber <= startSequenceNumber)
			{
				if (incomingCommand.unreliableSequenceNumber < startSequenceNumber)
					break;

				if ((incomingCommand.command.header.command & ENetProtocolCommand_Mask) != ENetProtocolCommand_SendUnreliableFragment ||
					totalLength != incomingCommand.packet->data.GetDataSize() || fragmentCount != incomingCommand.fragments.GetSize())
					return false;

				startCommand = &incomingCommand;
				break;
			}
		}

		if (startCommand)
		{
			if (!QueueIncomingCommand(*command, nullptr, totalLength, ENetPacketFlag_UnreliableFragment, fragmentCount))
				return false;
		}

		if (!startCommand->fragments.Test(fragmentNumber))
		{
			--startCommand->fragmentsRemaining;

			startCommand->fragments.Set(fragmentNumber, true);

			if (fragmentOffset + fragmentLength > startCommand->packet->data.GetDataSize())
				fragmentLength = startCommand->packet->data.GetDataSize() - fragmentOffset;

			std::memcpy(startCommand->packet->data.GetData() + NetPacket::HeaderSize + fragmentOffset, reinterpret_cast<const UInt8*>(command) + sizeof(ENetProtocolSendFragment), fragmentLength);

			if (startCommand->fragmentsRemaining <= 0)
				DispatchIncomingUnreliableCommands(channel);
		}

		return true;
	}

	bool ENetPeer::HandleSendUnsequenced(const ENetProtocol* command, UInt8** data)
	{
		if (command->header.channelID >= m_channels.size() || !IsConnected())
			return false;

		std::size_t dataLength = NetToHost(command->sendUnsequenced.dataLength);
		*data += dataLength;
		if (dataLength >= m_host->m_maximumPacketSize || *data < m_host->m_receivedData || *data > &m_host->m_receivedData[m_host->m_receivedDataLength])
			return false;

		UInt32 unsequencedGroup = NetToHost(command->sendUnsequenced.unsequencedGroup);
		UInt32 index = unsequencedGroup % ENetConstants::ENetPeer_UnsequencedWindowSize;

		if (unsequencedGroup < m_incomingUnsequencedGroup)
			unsequencedGroup += 0x10000;

		if (unsequencedGroup >= static_cast<UInt32>(m_incomingUnsequencedGroup) + ENetConstants::ENetPeer_UnsequencedWindows * ENetConstants::ENetPeer_UnsequencedWindowSize)
			return true;

		unsequencedGroup &= 0xFFFF;

		if (unsequencedGroup - index != m_incomingUnsequencedGroup)
		{
			m_incomingUnsequencedGroup = unsequencedGroup - index;

			m_unsequencedWindow.fill(0);
		}
		else if (m_unsequencedWindow[index / 32] & (1 << (index % 32)))
			return true;

		if (!QueueIncomingCommand(*command, reinterpret_cast<const UInt8*>(command) + sizeof(ENetProtocolSendUnsequenced), dataLength, ENetPacketFlag_Unsequenced, 0))
			return false;

		m_unsequencedWindow[index / 32] |= 1 << (index % 32);

		return true;
	}

	bool ENetPeer::HandleThrottleConfigure(const ENetProtocol* command)
	{
		if (!IsConnected())
			return false;

		m_packetThrottleInterval = NetToHost(command->throttleConfigure.packetThrottleInterval);
		m_packetThrottleAcceleration = NetToHost(command->throttleConfigure.packetThrottleAcceleration);
		m_packetThrottleDeceleration = NetToHost(command->throttleConfigure.packetThrottleDeceleration);

		return true;
	}

	bool ENetPeer::HandleVerifyConnect(const ENetProtocol* command, ENetEvent* event)
	{
		if (m_state != ENetPeerState::Connecting)
			return true;

		UInt32 channelCount = NetToHost(command->verifyConnect.channelCount);

		if (channelCount < ENetConstants::ENetProtocol_MinimumChannelCount || channelCount > ENetConstants::ENetProtocol_MaximumChannelCount ||
		    NetToHost(command->verifyConnect.packetThrottleInterval) != m_packetThrottleInterval ||
		    NetToHost(command->verifyConnect.packetThrottleAcceleration) != m_packetThrottleAcceleration ||
		    NetToHost(command->verifyConnect.packetThrottleDeceleration) != m_packetThrottleDeceleration ||
			command->verifyConnect.connectID != m_connectID)
		{
			m_eventData = 0;

			DispatchState(ENetPeerState::Zombie);

			return false;
		}

		RemoveSentReliableCommand(1, 0xFF);

		if (channelCount < m_channels.size())
			m_channels.resize(channelCount);

		m_outgoingPeerID = NetToHost(command->verifyConnect.outgoingPeerID);
		m_incomingSessionID = command->verifyConnect.incomingSessionID;
		m_outgoingSessionID = command->verifyConnect.outgoingSessionID;

		UInt32 mtu = Clamp<UInt32>(NetToHost(command->verifyConnect.mtu), ENetConstants::ENetProtocol_MinimumMTU, ENetConstants::ENetProtocol_MaximumMTU);
		m_mtu = std::min(m_mtu, mtu);

		UInt32 windowSize = Clamp<UInt32>(NetToHost(command->verifyConnect.windowSize), ENetConstants::ENetProtocol_MinimumWindowSize, ENetConstants::ENetProtocol_MaximumWindowSize);
		m_windowSize = std::min(m_windowSize, windowSize);

		m_incomingBandwidth = NetToHost(command->verifyConnect.incomingBandwidth);
		m_outgoingBandwidth = NetToHost(command->verifyConnect.outgoingBandwidth);

		m_host->NotifyConnect(this, event);
		return true;
	}

	void ENetPeer::InitIncoming(std::size_t channelCount, const IpAddress& address, ENetProtocolConnect& incomingCommand)
	{
		m_channels.resize(channelCount);
		m_address = address;

		m_connectID = incomingCommand.connectID;
		m_eventData = NetToHost(incomingCommand.data);
		m_incomingBandwidth = NetToHost(incomingCommand.incomingBandwidth);
		m_outgoingBandwidth = NetToHost(incomingCommand.outgoingBandwidth);
		m_packetThrottleInterval = NetToHost(incomingCommand.packetThrottleInterval);
		m_packetThrottleAcceleration = NetToHost(incomingCommand.packetThrottleAcceleration);
		m_packetThrottleDeceleration = NetToHost(incomingCommand.packetThrottleDeceleration);
		m_outgoingPeerID = NetToHost(incomingCommand.outgoingPeerID);
		m_state = ENetPeerState::AcknowledgingConnect;

		UInt8 incomingSessionId, outgoingSessionId;

		incomingSessionId = incomingCommand.incomingSessionID == 0xFF ? m_outgoingSessionID : incomingCommand.incomingSessionID;
		incomingSessionId = (incomingSessionId + 1) & (ENetProtocolHeaderSessionMask >> ENetProtocolHeaderSessionShift);
		if (incomingSessionId == m_outgoingSessionID)
			incomingSessionId = (incomingSessionId + 1) & (ENetProtocolHeaderSessionMask >> ENetProtocolHeaderSessionShift);
		m_outgoingSessionID = incomingSessionId;

		outgoingSessionId = incomingCommand.outgoingSessionID == 0xFF ? m_incomingSessionID : incomingCommand.outgoingSessionID;
		outgoingSessionId = (outgoingSessionId + 1) & (ENetProtocolHeaderSessionMask >> ENetProtocolHeaderSessionShift);
		if (outgoingSessionId == m_incomingSessionID)
			outgoingSessionId = (outgoingSessionId + 1) & (ENetProtocolHeaderSessionMask >> ENetProtocolHeaderSessionShift);
		m_incomingSessionID = outgoingSessionId;

		m_mtu = Clamp<UInt32>(NetToHost(incomingCommand.mtu), ENetConstants::ENetProtocol_MinimumMTU, ENetConstants::ENetProtocol_MaximumMTU);

		if (m_host->m_outgoingBandwidth == 0 && m_incomingBandwidth == 0)
			m_windowSize = ENetConstants::ENetProtocol_MaximumWindowSize;
		else if (m_host->m_outgoingBandwidth == 0 || m_incomingBandwidth == 0)
			m_windowSize = (std::max(m_host->m_outgoingBandwidth, m_incomingBandwidth) / ENetConstants::ENetPeer_WindowSizeScale) * ENetConstants::ENetProtocol_MinimumWindowSize;
		else
			m_windowSize = (std::min(m_host->m_outgoingBandwidth, m_incomingBandwidth) / ENetConstants::ENetPeer_WindowSizeScale) * ENetConstants::ENetProtocol_MinimumWindowSize;

		m_windowSize = Clamp<UInt32>(m_windowSize, ENetConstants::ENetProtocol_MinimumWindowSize, ENetConstants::ENetProtocol_MaximumWindowSize);
	}

	void ENetPeer::InitOutgoing(std::size_t channelCount, const IpAddress& address, UInt32 connectId, UInt32 windowSize)
	{
		m_channels.resize(channelCount);

		m_address = address;
		m_connectID = connectId;
		m_state = ENetPeerState::Connecting;
		m_windowSize = Clamp<UInt32>(windowSize, ENetConstants::ENetProtocol_MinimumWindowSize, ENetConstants::ENetProtocol_MaximumWindowSize);
	}

	void ENetPeer::OnConnect()
	{
		if (!IsConnected())
		{
			if (m_incomingBandwidth != 0)
				++m_host->m_bandwidthLimitedPeers;

			++m_host->m_connectedPeers;
		}
	}

	void ENetPeer::OnDisconnect()
	{
		if (IsConnected())
		{
			if (m_incomingBandwidth != 0)
				--m_host->m_bandwidthLimitedPeers;

			--m_host->m_connectedPeers;
		}
	}

	ENetProtocolCommand ENetPeer::RemoveSentReliableCommand(UInt16 reliableSequenceNumber, UInt8 channelId)
	{
		std::list<OutgoingCommand>* commandList = nullptr;

		bool found = false;
		auto currentCommand = m_sentReliableCommands.begin();
		commandList = &m_sentReliableCommands;
		for (; currentCommand != m_sentReliableCommands.end(); ++currentCommand)
		{
			found = true;

			if (currentCommand->reliableSequenceNumber == reliableSequenceNumber &&
				currentCommand->command.header.channelID == channelId)
				break;
		}

		bool wasSent = true;
		if (currentCommand == m_sentReliableCommands.end())
		{
			currentCommand = m_outgoingReliableCommands.begin();
			commandList = &m_sentReliableCommands;
			for (; currentCommand != m_outgoingReliableCommands.end(); ++currentCommand)
			{
				found = true;

				if (currentCommand->sendAttempts < 1)
					return ENetProtocolCommand_None;

				if (currentCommand->reliableSequenceNumber == reliableSequenceNumber &&
					currentCommand->command.header.channelID == channelId)
					break;
			}

			if (currentCommand == m_outgoingReliableCommands.end())
				return ENetProtocolCommand_None;

			wasSent = false;
		}

		if (!found) //< Really useful?
			return ENetProtocolCommand_None;

		if (channelId < m_channels.size())
		{
			Channel& channel = m_channels[channelId];

			UInt16 reliableWindow = reliableSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;
			if (channel.reliableWindows[reliableWindow] > 0)
			{
				--channel.reliableWindows[reliableWindow];
				if (!channel.reliableWindows[reliableWindow])
					channel.usedReliableWindows &= ~(1 << reliableWindow);
			}
		}

		ENetProtocolCommand commandNumber = static_cast<ENetProtocolCommand>(currentCommand->command.header.command & ENetProtocolCommand_Mask);

		if (currentCommand->packet && wasSent)
			m_reliableDataInTransit -= currentCommand->fragmentLength;

		commandList->erase(currentCommand);

		if (m_sentReliableCommands.empty())
			return commandNumber;

		currentCommand = m_sentReliableCommands.begin();
		m_nextTimeout = currentCommand->sentTime + currentCommand->roundTripTimeout;

		return commandNumber;
	}

	void ENetPeer::RemoveSentUnreliableCommands()
	{
		m_sentUnreliableCommands.clear();
	}

	void ENetPeer::ResetQueues()
	{
		m_host->RemoveFromDispatchQueue(this);

		m_acknowledgements.clear();
		m_dispatchedCommands.clear();
		m_outgoingReliableCommands.clear();
		m_outgoingUnreliableCommands.clear();
		m_sentReliableCommands.clear();
		m_sentUnreliableCommands.clear();

		m_channels.clear();
	}

	bool ENetPeer::QueueAcknowledgement(ENetProtocol*command, UInt16 sentTime)
	{
		if (command->header.channelID < m_channels.size())
		{
			Channel& channel = m_channels[command->header.channelID];

			UInt16 reliableWindow = command->header.reliableSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;
			UInt16 currentWindow  = channel.incomingReliableSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;

			if (command->header.reliableSequenceNumber < channel.incomingReliableSequenceNumber)
				reliableWindow += ENetConstants::ENetPeer_ReliableWindows;

			if (reliableWindow >= currentWindow + ENetConstants::ENetPeer_FreeReliableWindows - 1 && reliableWindow <= currentWindow + ENetConstants::ENetPeer_FreeReliableWindows)
				return false;
		}

		Acknowledgement acknowledgment;
		acknowledgment.command = *command;
		acknowledgment.sentTime = sentTime;

		m_outgoingDataTotal += sizeof(Acknowledgement);

		m_acknowledgements.emplace_back(acknowledgment);

		return true;
	}

	ENetPeer::IncomingCommmand* ENetPeer::QueueIncomingCommand(const ENetProtocol& command, const void* data, std::size_t dataLength, UInt32 flags, UInt32 fragmentCount)
	{
		static IncomingCommmand dummyCommand;

		UInt32 reliableSequenceNumber   = 0;
		UInt32 unreliableSequenceNumber = 0;
		UInt16 reliableWindow;
		UInt16 currentWindow;

		auto discardCommand = [&]() -> IncomingCommmand*
		{
			if (fragmentCount > 0)
				return nullptr; //< Error

			return &dummyCommand;
		};

		if (m_state == ENetPeerState::DisconnectLater)
			return discardCommand();

		Channel& channel = m_channels[command.header.channelID];

		if ((command.header.command & ENetProtocolCommand_Mask) != ENetProtocolCommand_SendUnsequenced)
		{
			reliableSequenceNumber = command.header.reliableSequenceNumber;
			reliableWindow = reliableSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;
			currentWindow = channel.incomingReliableSequenceNumber / ENetConstants::ENetPeer_ReliableWindowSize;

			if (reliableSequenceNumber < channel.incomingReliableSequenceNumber)
				reliableWindow += ENetConstants::ENetPeer_ReliableWindows;

			if (reliableWindow < currentWindow || reliableWindow >= currentWindow + ENetConstants::ENetPeer_FreeReliableWindows - 1)
				return discardCommand();
		}

		std::list<IncomingCommmand>* commandList = nullptr;
		std::list<IncomingCommmand>::reverse_iterator currentCommand;

		switch (command.header.command & ENetProtocolCommand_Mask)
		{
			case ENetProtocolCommand_SendFragment:
			case ENetProtocolCommand_SendReliable:
			{
				if (reliableSequenceNumber == channel.incomingReliableSequenceNumber)
					return discardCommand();

				commandList = &channel.incomingReliableCommands;

				for (currentCommand = channel.incomingReliableCommands.rbegin(); currentCommand != channel.incomingReliableCommands.rend(); ++currentCommand)
				{
					IncomingCommmand& incomingCommand = *currentCommand;

					if (reliableSequenceNumber >= channel.incomingReliableSequenceNumber)
					{
						if (incomingCommand.reliableSequenceNumber < channel.incomingReliableSequenceNumber)
							continue;
					}
					else
						if (incomingCommand.reliableSequenceNumber >= channel.incomingReliableSequenceNumber)
							break;

					if (incomingCommand.reliableSequenceNumber <= reliableSequenceNumber)
					{
						if (incomingCommand.reliableSequenceNumber < reliableSequenceNumber)
							break;

						return discardCommand();
					}
				}
				break;
			}

			case ENetProtocolCommand_SendUnreliable:
			case ENetProtocolCommand_SendUnreliableFragment:
			{
				unreliableSequenceNumber = NetToHost(command.sendUnreliable.unreliableSequenceNumber);

				if (reliableSequenceNumber == channel.incomingReliableSequenceNumber && unreliableSequenceNumber <= channel.incomingUnreliableSequenceNumber)
					return discardCommand();

				commandList = &channel.incomingUnreliableCommands;

				for (currentCommand = channel.incomingUnreliableCommands.rbegin(); currentCommand != channel.incomingUnreliableCommands.rend(); ++currentCommand)
				{
					IncomingCommmand& incomingCommand = *currentCommand;

					if ((command.header.command & ENetProtocolCommand_Mask) == ENetProtocolCommand_SendUnsequenced) //< wtf
						continue;

					if (reliableSequenceNumber >= channel.incomingReliableSequenceNumber)
					{
						if (incomingCommand.reliableSequenceNumber < channel.incomingReliableSequenceNumber)
							continue;
					}
					else
						if (incomingCommand.reliableSequenceNumber >= channel.incomingReliableSequenceNumber)
							break;

					if (incomingCommand.reliableSequenceNumber < reliableSequenceNumber)
						break;

					if (incomingCommand.reliableSequenceNumber > reliableSequenceNumber)
						continue;

					if (incomingCommand.unreliableSequenceNumber <= unreliableSequenceNumber)
					{
						if (incomingCommand.unreliableSequenceNumber < unreliableSequenceNumber)
							break;

						return discardCommand();
					}
				}
				break;
			}

			case ENetProtocolCommand_SendUnsequenced:
			{
				commandList = &channel.incomingUnreliableCommands;

				currentCommand = channel.incomingUnreliableCommands.rend();
				break;
			}

			default:
				return discardCommand();
		}

		if (m_totalWaitingData >= m_host->m_maximumWaitingData)
			return nullptr;

		ENetPacket* packet = m_packetPool.New<ENetPacket>();
		packet->flags = flags;
		packet->data.Reset(0, data, dataLength);
		packet->owner = &m_packetPool;

		IncomingCommmand incomingCommand;
		incomingCommand.reliableSequenceNumber = command.header.reliableSequenceNumber;
		incomingCommand.unreliableSequenceNumber = unreliableSequenceNumber & 0xFFFF;
		incomingCommand.command = command;
		incomingCommand.packet = packet;
		incomingCommand.fragments.Resize(fragmentCount);
		incomingCommand.fragmentsRemaining = fragmentCount;

		if (packet)
			m_totalWaitingData += packet->data.GetDataSize();

		auto it = commandList->insert(currentCommand.base(), incomingCommand);

		switch (command.header.command & ENetProtocolCommand_Mask)
		{
			case ENetProtocolCommand_SendFragment:
			case ENetProtocolCommand_SendReliable:
				DispatchIncomingReliableCommands(channel);
				break;

			default:
				DispatchIncomingUnreliableCommands(channel);
				break;
		}

		return &(*it);
	}

	void ENetPeer::QueueOutgoingCommand(ENetProtocol& command, ENetPacketRef packet, UInt32 offset, UInt16 length)
	{
		OutgoingCommand outgoingCommand;
		outgoingCommand.command = command;
		outgoingCommand.fragmentLength = length;
		outgoingCommand.fragmentOffset = offset;
		outgoingCommand.packet = packet;

		SetupOutgoingCommand(outgoingCommand);
	}

	void ENetPeer::SetupOutgoingCommand(OutgoingCommand& outgoingCommand)
	{
		m_outgoingDataTotal += ENetHost::GetCommandSize(outgoingCommand.command.header.command) + outgoingCommand.fragmentLength;

		if (outgoingCommand.command.header.channelID == 0xFF)
		{
			++m_outgoingReliableSequenceNumber;

			outgoingCommand.reliableSequenceNumber = m_outgoingReliableSequenceNumber;
			outgoingCommand.unreliableSequenceNumber = 0;
		}
		else
		{
			Channel* channel = &m_channels[outgoingCommand.command.header.channelID];
			if (outgoingCommand.command.header.command & ENetProtocolFlag_Acknowledge)
			{
				++channel->outgoingReliableSequenceNumber;
				channel->outgoingUnreliableSequenceNumber = 0;

				outgoingCommand.reliableSequenceNumber = channel->outgoingReliableSequenceNumber;
				outgoingCommand.unreliableSequenceNumber = 0;
			}
			else if (outgoingCommand.command.header.command & ENetProtocolFlag_Unsequenced)
			{
				++m_outgoingUnsequencedGroup;

				outgoingCommand.reliableSequenceNumber = 0;
				outgoingCommand.unreliableSequenceNumber = 0;
			}
			else
			{
				if (outgoingCommand.fragmentOffset == 0)
					++channel->outgoingUnreliableSequenceNumber;

				outgoingCommand.reliableSequenceNumber = channel->outgoingReliableSequenceNumber;
				outgoingCommand.unreliableSequenceNumber = channel->outgoingUnreliableSequenceNumber;
			}
		}

		outgoingCommand.sendAttempts = 0;
		outgoingCommand.sentTime = 0;
		outgoingCommand.roundTripTimeout = 0;
		outgoingCommand.roundTripTimeoutLimit = 0;
		outgoingCommand.command.header.reliableSequenceNumber = HostToNet(outgoingCommand.reliableSequenceNumber);

		switch (outgoingCommand.command.header.command & ENetProtocolCommand_Mask)
		{
			case ENetProtocolCommand_SendUnreliable:
				outgoingCommand.command.sendUnreliable.unreliableSequenceNumber = HostToNet(outgoingCommand.unreliableSequenceNumber);
				break;

			case ENetProtocolCommand_SendUnsequenced:
				outgoingCommand.command.sendUnsequenced.unsequencedGroup = HostToNet(m_outgoingUnsequencedGroup);
				break;

			default:
				break;
		}

		if (outgoingCommand.command.header.command & ENetProtocolFlag_Acknowledge)
			m_outgoingReliableCommands.emplace_back(outgoingCommand);
		else
			m_outgoingUnreliableCommands.emplace_back(outgoingCommand);
	}

	int ENetPeer::Throttle(UInt32 rtt)
	{
		if (m_lastRoundTripTime <= m_lastRoundTripTimeVariance)
			m_packetThrottle = m_packetThrottleLimit;
		else
		{
			if (rtt < m_lastRoundTripTime)
			{
				m_packetThrottle = std::max(m_packetThrottle + m_packetThrottleAcceleration, m_packetThrottleLimit);
				return 1;
			}
			else if (rtt > m_lastRoundTripTime + 2 * m_lastRoundTripTimeVariance)
			{
				if (m_packetThrottle > m_packetThrottleDeceleration)
					m_packetThrottle -= m_packetThrottleDeceleration;
				else
					m_packetThrottle = 0;

				return -1;
			}
		}

		return 0;
	}
}

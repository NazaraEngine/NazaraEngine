/*
	Copyright(c) 2002 - 2016 Lee Salzman

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENETPEER_HPP
#define NAZARA_ENETPEER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/MemoryPool.hpp>
#include <Nazara/Network/ENetPacket.hpp>
#include <Nazara/Network/ENetPeer.hpp>
#include <Nazara/Network/ENetProtocol.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <Nazara/Network/UdpSocket.hpp>
#include <array>
#include <list>
#include <vector>

namespace Nz
{
	class ENetHost;

	class NAZARA_NETWORK_API ENetPeer
	{
		friend ENetHost;
		friend struct PacketRef;

		public:
			ENetPeer(ENetHost* host, UInt16 peerId);
			ENetPeer(const ENetPeer&) = delete;
			ENetPeer(ENetPeer&&) = default;
			~ENetPeer() = default;

			void Disconnect(UInt32 data);
			void DisconnectLater(UInt32 data);
			void DisconnectNow(UInt32 data);

			inline const IpAddress& GetAddress() const;
			inline UInt16 GetPeerId() const;
			inline ENetPeerState GetState() const;

			inline bool HasPendingCommands();

			inline bool IsConnected() const;

			void Ping();

			bool Receive(ENetPacketRef* packet, UInt8* channelId);
			void Reset();

			bool Send(UInt8 channelId, ENetPacketRef packetRef);
			bool Send(UInt8 channelId, ENetPacketFlags flags, NetPacket&& packet);

			void ThrottleConfigure(UInt32 interval, UInt32 acceleration, UInt32 deceleration);

			ENetPeer& operator=(const ENetPeer&) = delete;
			ENetPeer& operator=(ENetPeer&&) = default;

		private:
			void InitIncoming(std::size_t channelCount, const IpAddress& address, ENetProtocolConnect& incomingCommand);
			void InitOutgoing(std::size_t channelCount, const IpAddress& address, UInt32 connectId, UInt32 windowSize);

			struct Acknowledgement;
			struct Channel;
			struct IncomingCommmand;
			struct OutgoingCommand;

			inline void ChangeState(ENetPeerState state);

			bool CheckTimeouts(ENetEvent* event);

			void DispatchState(ENetPeerState state);

			void DispatchIncomingReliableCommands(Channel& channel);
			void DispatchIncomingUnreliableCommands(Channel& channel);

			bool HandleAcknowledge(const ENetProtocol* command, ENetEvent* event);
			bool HandleBandwidthLimit(const ENetProtocol* command);
			bool HandleDisconnect(const ENetProtocol* command);
			bool HandlePing(const ENetProtocol* command);
			bool HandleSendFragment(const ENetProtocol* command, UInt8** data);
			bool HandleSendReliable(const ENetProtocol* command, UInt8** data);
			bool HandleSendUnreliable(const ENetProtocol* command, UInt8** data);
			bool HandleSendUnreliableFragment(const ENetProtocol* command, UInt8** data);
			bool HandleSendUnsequenced(const ENetProtocol* command, UInt8** data);
			bool HandleThrottleConfigure(const ENetProtocol* command);
			bool HandleVerifyConnect(const ENetProtocol* command, ENetEvent* event);

			void OnConnect();
			void OnDisconnect();

			ENetProtocolCommand RemoveSentReliableCommand(UInt16 reliableSequenceNumber, UInt8 channelId);
			void RemoveSentUnreliableCommands();

			void ResetQueues();

			bool QueueAcknowledgement(ENetProtocol* command, UInt16 sentTime);
			IncomingCommmand* QueueIncomingCommand(const ENetProtocol& command, const void* data, std::size_t dataLength, UInt32 flags, UInt32 fragmentCount);
			inline void QueueOutgoingCommand(ENetProtocol& command);
			void QueueOutgoingCommand(ENetProtocol& command, ENetPacketRef packet, UInt32 offset, UInt16 length);

			void SetupOutgoingCommand(OutgoingCommand& outgoingCommand);

			int Throttle(UInt32 rtt);

			struct Acknowledgement
			{
				ENetProtocol command;
				UInt32 sentTime;
			};

			struct Channel
			{
				Channel()
				{
					incomingReliableSequenceNumber = 0;
					incomingUnreliableSequenceNumber = 0;
					outgoingReliableSequenceNumber = 0;
					outgoingUnreliableSequenceNumber = 0;
					usedReliableWindows = 0;
					reliableWindows.fill(0);
				}

				std::array<UInt16, ENetPeer_ReliableWindows> reliableWindows;
				std::list<IncomingCommmand>                  incomingReliableCommands;
				std::list<IncomingCommmand>                  incomingUnreliableCommands;
				UInt16                                       incomingReliableSequenceNumber;
				UInt16                                       incomingUnreliableSequenceNumber;
				UInt16                                       outgoingReliableSequenceNumber;
				UInt16                                       outgoingUnreliableSequenceNumber;
				UInt16                                       usedReliableWindows;
			};

			struct IncomingCommmand
			{
				ENetProtocol  command;
				Bitset<>      fragments;
				ENetPacketRef packet;
				UInt16        reliableSequenceNumber;
				UInt16        unreliableSequenceNumber;
				UInt32        fragmentsRemaining;
			};

			struct OutgoingCommand
			{
				ENetProtocol  command;
				ENetPacketRef packet;
				UInt16        fragmentLength;
				UInt16        reliableSequenceNumber;
				UInt16        sendAttempts;
				UInt16        unreliableSequenceNumber;
				UInt32        fragmentOffset;
				UInt32        roundTripTimeout;
				UInt32        roundTripTimeoutLimit;
				UInt32        sentTime;
			};

			static constexpr std::size_t unsequencedWindow = ENetPeer_ReliableWindowSize / 32;

			ENetHost*                             m_host;
			IpAddress                             m_address; /**< Internet address of the peer */
			std::array<UInt32, unsequencedWindow> m_unsequencedWindow;
			std::list<Acknowledgement>            m_acknowledgements;
			std::list<IncomingCommmand>           m_dispatchedCommands;
			std::list<OutgoingCommand>            m_outgoingReliableCommands;
			std::list<OutgoingCommand>            m_outgoingUnreliableCommands;
			std::list<OutgoingCommand>            m_sentReliableCommands;
			std::list<OutgoingCommand>            m_sentUnreliableCommands;
			std::size_t                           m_totalWaitingData;
			std::vector<Channel>                  m_channels;
			MemoryPool                            m_packetPool;
			ENetPeerState                         m_state;
			UInt8                                 m_incomingSessionID;
			UInt8                                 m_outgoingSessionID;
			UInt16                                m_incomingPeerID;
			UInt16                                m_incomingUnsequencedGroup;
			UInt16                                m_outgoingPeerID;
			UInt16                                m_outgoingReliableSequenceNumber;
			UInt16                                m_outgoingUnsequencedGroup;
			UInt32                                m_connectID;
			UInt32                                m_earliestTimeout;
			UInt32                                m_eventData;
			UInt32                                m_highestRoundTripTimeVariance;
			UInt32                                m_incomingBandwidth;  /**< Downstream bandwidth of the client in bytes/second */
			UInt32                                m_incomingBandwidthThrottleEpoch;
			UInt32                                m_incomingDataTotal;
			UInt32                                m_lastReceiveTime;
			UInt32                                m_lastRoundTripTime;
			UInt32                                m_lastRoundTripTimeVariance;
			UInt32                                m_lastSendTime;
			UInt32                                m_lowestRoundTripTime;
			UInt32                                m_mtu;
			UInt32                                m_nextTimeout;
			UInt32                                m_outgoingBandwidth;  /**< Upstream bandwidth of the client in bytes/second */
			UInt32                                m_outgoingBandwidthThrottleEpoch;
			UInt32                                m_outgoingDataTotal;
			UInt32                                m_packetLoss;          /**< mean packet loss of reliable packets as a ratio with respect to the constant ENET_PEER_PACKET_LOSS_SCALE */
			UInt32                                m_packetLossEpoch;
			UInt32                                m_packetLossVariance;
			UInt32                                m_packetThrottle;
			UInt32                                m_packetThrottleAcceleration;
			UInt32                                m_packetThrottleCounter;
			UInt32                                m_packetThrottleDeceleration;
			UInt32                                m_packetThrottleEpoch;
			UInt32                                m_packetThrottleInterval;
			UInt32                                m_packetThrottleLimit;
			UInt32                                m_packetsLost;
			UInt32                                m_packetsSent;
			UInt32                                m_pingInterval;
			UInt32                                m_reliableDataInTransit;
			UInt32                                m_roundTripTime;            /**< mean round trip time (RTT), in milliseconds, between sending a reliable packet and receiving its acknowledgment */
			UInt32                                m_roundTripTimeVariance;
			UInt32                                m_timeoutLimit;
			UInt32                                m_timeoutMaximum;
			UInt32                                m_timeoutMinimum;
			UInt32                                m_windowSize;
			UInt64                                m_totalPacketLost;
			UInt64                                m_totalPacketSent;
	};
}

#include <Nazara/Network/ENetPeer.inl>

#endif // NAZARA_ENETPEER_HPP

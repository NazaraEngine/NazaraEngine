// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RUDPSERVER_HPP
#define NAZARA_RUDPSERVER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <Nazara/Network/RUdpMessage.hpp>
#include <Nazara/Network/UdpSocket.hpp>
#include <deque>
#include <queue>
#include <random>
#include <set>
#include <unordered_map>

namespace Nz
{
	class NAZARA_NETWORK_API RUdpConnection
	{
		friend class Network;

		public:
			using SequenceIndex = UInt16;

			RUdpConnection();
			RUdpConnection(const RUdpConnection&) = delete;
			RUdpConnection(RUdpConnection&&) = default;
			~RUdpConnection() = default;

			inline void Close();

			bool Connect(const IpAddress& remoteAddress);
			bool Connect(const String& hostName, NetProtocol protocol = NetProtocol_Any, const String& service = "http", ResolveError* error = nullptr);
			inline void Disconnect();

			inline IpAddress GetBoundAddress() const;
			inline UInt16 GetBoundPort() const;
			inline SocketError GetLastError() const;

			inline bool Listen(NetProtocol protocol, UInt16 port = 64266);
			bool Listen(const IpAddress& address);

			bool PollMessage(RUdpMessage* message);

			bool Send(const IpAddress& clientIp, PacketPriority priority, PacketReliability reliability, const NetPacket& packet);

			inline void SetProtocolId(UInt32 protocolId);
			inline void SetTimeBeforeAck(UInt32 ms);

			inline void SimulateNetwork(double packetLoss);

			void Update();

			RUdpConnection& operator=(const RUdpConnection&) = delete;
			RUdpConnection& operator=(RUdpConnection&&) = default;

			static constexpr std::size_t MessageHeader = sizeof(UInt16) + 2 * sizeof(SequenceIndex) + sizeof(UInt32); //< Protocol ID (begin) + Sequence ID + Remote Sequence ID + Ack bitfield
			static constexpr std::size_t MessageFooter = sizeof(UInt16); //< Protocol ID (end)

			// Signals:
			NazaraSignal(OnConnectedToPeer,  RUdpConnection* /*connection*/);
			NazaraSignal(OnPeerAcknowledged, RUdpConnection* /*connection*/, const IpAddress& /*adress*/);
			NazaraSignal(OnPeerConnection,   RUdpConnection* /*connection*/, const IpAddress& /*adress*/);
			NazaraSignal(OnPeerDisconnected, RUdpConnection* /*connection*/, const IpAddress& /*adress*/);

		private:
			struct PeerData;
			struct PendingAckPacket;
			struct PendingPacket;

			enum PeerState
			{
				PeerState_Aknowledged, //< A connection request from this peer has been received, we're waiting for another packet to validate
				PeerState_Connected,   //< Connection is working in both-ways
				PeerState_Connecting,  //< A connection request has been made
				PeerState_WillAck      //< Connected, received one or more packets and has no packets to send, waiting before sending an empty ack packet
			};

			void DisconnectPeer(std::size_t peerIndex);
			void EnqueuePacket(PeerData& peer, PacketPriority priority, PacketReliability reliability, const NetPacket& packet);
			void EnqueuePacketInternal(PeerData& peer, PacketPriority priority, PacketReliability reliability, NetPacket&& data);
			bool InitSocket(NetProtocol protocol);
			void ProcessAcks(PeerData& peer, SequenceIndex lastAck, UInt32 ackBits);
			PeerData& RegisterPeer(const IpAddress& address, PeerState state);
			void OnClientRequestingConnection(const IpAddress& address, SequenceIndex sequenceId, UInt64 token);
			void OnPacketLost(PeerData& peer, PendingAckPacket&& packet);
			void OnPacketReceived(const IpAddress& peerIp, NetPacket&& packet);
			void SendPacket(PeerData& peer, PendingPacket&& packet);

			static inline unsigned int ComputeSequenceDifference(SequenceIndex sequence, SequenceIndex sequence2);
			static inline bool HasPendingPackets(PeerData& peer);
			static bool Initialize();
			static inline bool IsAckMoreRecent(SequenceIndex ack, SequenceIndex ack2);
			static inline bool IsReliable(PacketReliability reliability);
			static void Uninitialize();

			struct PendingPacket
			{
				PacketPriority priority;
				PacketReliability reliability;
				NetPacket data;
			};

			struct PendingAckPacket
			{
				PacketPriority priority;
				PacketReliability reliability;
				NetPacket data;
				SequenceIndex sequenceId;
				UInt64 timeSent;
			};

			struct PeerData //TODO: Move this to RUdpClient
			{
				PeerData() = default;
				PeerData(PeerData&& other) = default;
				PeerData& operator=(PeerData&& other) = default;

				std::array<std::vector<PendingPacket>, PacketPriority_Max + 1> pendingPackets;
				std::deque<PendingAckPacket> pendingAckQueue;
				std::set<UInt16> receivedQueue;
				std::size_t index;
				PeerState state;
				IpAddress address;
				SequenceIndex localSequence;
				SequenceIndex remoteSequence;
				UInt32 roundTripTime;
				UInt64 lastPacketTime;
				UInt64 lastPingTime;
				UInt64 stateData1;
			};

			std::bernoulli_distribution m_packetLossProbability;
			std::queue<RUdpMessage> m_receivedMessages;
			std::size_t m_peerIterator;
			std::unordered_map<IpAddress, std::size_t> m_peerByIP;
			std::vector<PeerData> m_peers;
			Bitset<UInt64> m_activeClients;
			Clock m_clock;
			SocketError m_lastError;
			UdpSocket m_socket;
			UInt32 m_forceAckSendTime;
			UInt32 m_pingInterval;
			UInt32 m_protocol;
			UInt32 m_timeBeforePing;
			UInt32 m_timeBeforeTimeOut;
			UInt64 m_currentTime;
			bool m_isSimulationEnabled;
			bool m_shouldAcceptConnections;

			static std::mt19937_64 s_randomGenerator;
	};
}

#include <Nazara/Network/RUdpConnection.inl>

#endif // NAZARA_RUDPSERVER_HPP

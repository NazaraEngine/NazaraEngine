// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ENETHOST_HPP
#define NAZARA_ENETHOST_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/MemoryPool.hpp>
#include <Nazara/Network/ENetHost.hpp>
#include <Nazara/Network/ENetProtocol.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Network/NetBuffer.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <Nazara/Network/SocketPoller.hpp>
#include <Nazara/Network/UdpSocket.hpp>
#include <deque>
#include <queue>
#include <random>
#include <set>
#include <unordered_map>

namespace Nz
{
	class ENetPeer;

	class NAZARA_NETWORK_API ENetHost
	{
		friend ENetPeer;
		friend class Network;

		public:
			inline ENetHost();
			ENetHost(const ENetHost&) = delete;
			ENetHost(ENetHost&&) = default;
			inline ~ENetHost();

			void Broadcast(UInt8 channelId, ENetPacketFlags flags, NetPacket&& packet);

			bool CheckEvents(ENetEvent* event);

			ENetPeer* Connect(const IpAddress& remoteAddress, std::size_t channelCount = 0, UInt32 data = 0);
			ENetPeer* Connect(const String& hostName, NetProtocol protocol = NetProtocol_Any, const String& service = "http", ResolveError* error = nullptr, std::size_t channelCount = 0, UInt32 data = 0);

			inline bool Create(NetProtocol protocol, UInt16 port, std::size_t peerCount, std::size_t channelCount = 0);
			bool Create(const IpAddress& address, std::size_t peerCount, std::size_t channelCount = 0);
			bool Create(const IpAddress& address, std::size_t peerCount, std::size_t channelCount, UInt32 incomingBandwidth, UInt32 outgoingBandwidth);
			void Destroy();

			void Flush();

			inline UInt32 GetServiceTime() const;

			int Service(ENetEvent* event, UInt32 timeout);

			void SimulateNetwork(double packetLossProbability, UInt16 minDelay, UInt16 maxDelay);

			ENetHost& operator=(const ENetHost&) = delete;
			ENetHost& operator=(ENetHost&&) = default;

		private:
			bool InitSocket(const IpAddress& address);

			void AddToDispatchQueue(ENetPeer* peer);
			void RemoveFromDispatchQueue(ENetPeer* peer);

			bool DispatchIncomingCommands(ENetEvent* event);

			ENetPeer* HandleConnect(ENetProtocolHeader* header, ENetProtocol* command);
			bool HandleIncomingCommands(ENetEvent* event);

			int ReceiveIncomingCommands(ENetEvent* event);

			void NotifyConnect(ENetPeer* peer, ENetEvent* event);
			void NotifyDisconnect(ENetPeer*, ENetEvent* event);

			void SendAcknowledgements(ENetPeer* peer);
			bool SendReliableOutgoingCommands(ENetPeer* peer);
			int SendOutgoingCommands(ENetEvent* event, bool checkForTimeouts);
			void SendUnreliableOutgoingCommands(ENetPeer* peer);

			void ThrottleBandwidth();

			static std::size_t GetCommandSize(UInt8 commandNumber);
			static bool Initialize();
			static void Uninitialize();

			struct PendingPacket
			{
				IpAddress from;
				NetPacket data;
				UInt32 deliveryTime;
			};

			std::array<ENetProtocol, ENetConstants::ENetProtocol_MaximumPacketCommands> m_commands;
			std::array<NetBuffer, ENetConstants::ENetProtocol_MaximumPacketCommands * 2 + 1> m_buffers;
			std::array<UInt8, ENetConstants::ENetProtocol_MaximumMTU> m_packetData[2];
			std::bernoulli_distribution m_packetLossProbability;
			std::size_t m_bandwidthLimitedPeers;
			std::size_t m_bufferCount;
			std::size_t m_channelLimit;
			std::size_t m_commandCount;
			std::size_t m_duplicatePeers;
			std::size_t m_maximumPacketSize;
			std::size_t m_maximumWaitingData;
			std::size_t m_packetSize;
			std::size_t m_peerCount;
			std::size_t m_receivedDataLength;
			std::uniform_int_distribution<UInt16> m_packetDelayDistribution;
			std::vector<ENetPeer> m_peers;
			std::vector<PendingPacket> m_pendingPackets;
			UInt8* m_receivedData;
			Bitset<UInt64> m_dispatchQueue;
			MemoryPool m_packetPool;
			IpAddress m_address;
			IpAddress m_receivedAddress;
			SocketPoller m_poller;
			UdpSocket m_socket;
			UInt16 m_headerFlags;
			UInt32 m_bandwidthThrottleEpoch;
			UInt32 m_connectedPeers;
			UInt32 m_mtu;
			UInt32 m_randomSeed;
			UInt32 m_incomingBandwidth;
			UInt32 m_outgoingBandwidth;
			UInt32 m_serviceTime;
			UInt32 m_totalSentData;
			UInt32 m_totalSentPackets;
			UInt32 m_totalReceivedData;
			UInt32 m_totalReceivedPackets;
			bool m_continueSending;
			bool m_isSimulationEnabled;
			bool m_recalculateBandwidthLimits;

			static std::mt19937 s_randomGenerator;
			static std::mt19937_64 s_randomGenerator64;
	};
}

#include <Nazara/Network/ENetHost.inl>

#endif // NAZARA_RUDPSERVER_HPP

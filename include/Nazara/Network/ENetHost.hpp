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

#ifndef NAZARA_ENETHOST_HPP
#define NAZARA_ENETHOST_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/MemoryPool.hpp>
#include <Nazara/Network/ENetCompressor.hpp>
#include <Nazara/Network/ENetPeer.hpp>
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
			bool Create(const IpAddress& listenAddress, std::size_t peerCount, std::size_t channelCount = 0);
			bool Create(const IpAddress& listenAddress, std::size_t peerCount, std::size_t channelCount, UInt32 incomingBandwidth, UInt32 outgoingBandwidth);
			void Destroy();

			void Flush();

			inline Nz::IpAddress GetBoundAddress() const;
			inline UInt32 GetServiceTime() const;

			int Service(ENetEvent* event, UInt32 timeout);

			inline void SetCompressor(std::unique_ptr<ENetCompressor>&& compressor);

			void SimulateNetwork(double packetLossProbability, UInt16 minDelay, UInt16 maxDelay);

			ENetHost& operator=(const ENetHost&) = delete;
			ENetHost& operator=(ENetHost&&) = default;

		private:
			ENetPacketRef AllocatePacket(ENetPacketFlags flags);
			inline ENetPacketRef AllocatePacket(ENetPacketFlags flags, NetPacket&& data);

			bool InitSocket(const IpAddress& address);

			void AddToDispatchQueue(ENetPeer* peer);
			void RemoveFromDispatchQueue(ENetPeer* peer);

			bool DispatchIncomingCommands(ENetEvent* event);

			ENetPeer* HandleConnect(ENetProtocolHeader* header, ENetProtocol* command);
			bool HandleIncomingCommands(ENetEvent* event);

			int ReceiveIncomingCommands(ENetEvent* event);

			void NotifyConnect(ENetPeer* peer, ENetEvent* event, bool incoming);
			void NotifyDisconnect(ENetPeer*, ENetEvent* event);

			void SendAcknowledgements(ENetPeer* peer);
			bool SendReliableOutgoingCommands(ENetPeer* peer);
			int SendOutgoingCommands(ENetEvent* event, bool checkForTimeouts);
			void SendUnreliableOutgoingCommands(ENetPeer* peer);

			void ThrottleBandwidth();

			static std::size_t GetCommandSize(UInt8 commandNumber);
			static bool Initialize();
			static void Uninitialize();

			struct PendingIncomingPacket
			{
				IpAddress from;
				NetPacket data;
				UInt32 deliveryTime;
			};

			struct PendingOutgoingPacket
			{
				IpAddress to;
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
			std::unique_ptr<ENetCompressor> m_compressor;
			std::vector<ENetPeer> m_peers;
			std::vector<PendingIncomingPacket> m_pendingIncomingPackets;
			std::vector<PendingOutgoingPacket> m_pendingOutgoingPackets;
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
			UInt32 m_totalSentPackets;
			UInt32 m_totalReceivedPackets;
			UInt64 m_totalSentData;
			UInt64 m_totalReceivedData;
			bool m_allowsIncomingConnections;
			bool m_continueSending;
			bool m_isSimulationEnabled;
			bool m_recalculateBandwidthLimits;

			static std::mt19937 s_randomGenerator;
			static std::mt19937_64 s_randomGenerator64;
	};
}

#include <Nazara/Network/ENetHost.inl>

#endif // NAZARA_ENETHOST_HPP

// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/ENetHost.hpp>
#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline ENetHost::ENetHost() :
	m_packetPool(sizeof(ENetPacket)),
	m_isUsingDualStack(false),
	m_isSimulationEnabled(false)
	{
	}

	inline ENetHost::~ENetHost()
	{
		Destroy();
	}

	inline void ENetHost::AllowsIncomingConnections(bool allow)
	{
		NazaraAssert(m_address.IsValid() && !m_address.IsLoopback(), "Only server hosts can allow incoming connections");

		m_allowsIncomingConnections = allow;
	}

	inline bool ENetHost::Create(NetProtocol protocol, UInt16 port, std::size_t peerCount, std::size_t channelCount)
	{
		NazaraAssert(protocol != NetProtocol::Unknown, "Invalid protocol");

		IpAddress any;
		switch (protocol)
		{
			case NetProtocol::Unknown:
				NazaraInternalError("Invalid protocol");
				return false;

			case NetProtocol::IPv4:
				any = IpAddress::AnyIpV4;
				break;

			case NetProtocol::Any:
				m_isUsingDualStack = true;
				// fallthrough
			case NetProtocol::IPv6:
				any = IpAddress::AnyIpV6;
				break;
		}

		any.SetPort(port);
		return Create(any, peerCount, channelCount);
	}

	inline void ENetHost::Destroy()
	{
		m_poller.Clear();
		m_peers.clear();
		m_socket.Close();
	}

	inline bool ENetHost::DoesAllowIncomingConnections() const
	{
		return m_allowsIncomingConnections;
	}

	inline IpAddress ENetHost::GetBoundAddress() const
	{
		return m_address;
	}

	inline UInt32 ENetHost::GetServiceTime() const
	{
		return m_serviceTime;
	}

	inline UInt32 ENetHost::GetTotalReceivedPackets() const
	{
		return m_totalReceivedPackets;
	}

	inline UInt64 ENetHost::GetTotalReceivedData() const
	{
		return m_totalReceivedData;
	}

	inline UInt64 ENetHost::GetTotalSentData() const
	{
		return m_totalSentData;
	}

	inline UInt32 ENetHost::GetTotalSentPackets() const
	{
		return m_totalSentPackets;
	}

	inline void ENetHost::SetCompressor(std::unique_ptr<ENetCompressor>&& compressor)
	{
		m_compressor = std::move(compressor);
	}

	inline ENetPacketRef ENetHost::AllocatePacket(ENetPacketFlags flags, NetPacket&& data)
	{
		ENetPacketRef ref = AllocatePacket(flags);
		ref->data = std::move(data);

		return ref;
	}

	inline void ENetHost::UpdateServiceTime()
	{
		// Compute service time as microseconds for extra precision
		m_serviceTime = static_cast<UInt32>(GetElapsedMicroseconds() / 1000);
	}
}

#include <Nazara/Network/DebugOff.hpp>

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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

	inline bool ENetHost::Create(NetProtocol protocol, UInt16 port, std::size_t peerCount, std::size_t channelCount)
	{
		NazaraAssert(protocol != NetProtocol_Unknown, "Invalid protocol");

		IpAddress any;
		switch (protocol)
		{
			case NetProtocol_Unknown:
				NazaraInternalError("Invalid protocol");
				return false;

			case NetProtocol_IPv4:
				any = IpAddress::AnyIpV4;
				break;

			case NetProtocol_Any:
				m_isUsingDualStack = true;
				// fallthrough
			case NetProtocol_IPv6:
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

	inline IpAddress ENetHost::GetBoundAddress() const
	{
		return m_address;
	}

	inline UInt32 ENetHost::GetServiceTime() const
	{
		return m_serviceTime;
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

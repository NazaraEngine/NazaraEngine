// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/ENetHost.hpp>
#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline ENetHost::ENetHost() :
	m_packetPool(sizeof(ENetPacket)),
	m_isSimulationEnabled(false)
	{
	}

	inline ENetHost::~ENetHost()
	{
		Destroy();
	}

	inline bool ENetHost::Create(NetProtocol protocol, UInt16 port, std::size_t peerCount, std::size_t channelCount)
	{
		NazaraAssert(protocol != NetProtocol_Any, "Any protocol not supported for Listen"); //< TODO
		NazaraAssert(protocol != NetProtocol_Unknown, "Invalid protocol");

		IpAddress any;
		switch (protocol)
		{
			case NetProtocol_Any:
			case NetProtocol_Unknown:
				NazaraInternalError("Invalid protocol Any at this point");
				return false;

			case NetProtocol_IPv4:
				any = IpAddress::AnyIpV4;
				break;

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

	inline UInt32 Nz::ENetHost::GetServiceTime() const
	{
		return m_serviceTime;
	}

	inline ENetPacketRef ENetHost::AllocatePacket(ENetPacketFlags flags, NetPacket&& data)
	{
		ENetPacketRef ref = AllocatePacket(flags);
		ref->data = std::move(data);

		return ref;
	}
}

#include <Nazara/Network/DebugOff.hpp>

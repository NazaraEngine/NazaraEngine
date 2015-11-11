// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline UdpSocket::UdpSocket() :
	AbstractSocket(SocketType_UDP)
	{
	}

	inline UdpSocket::UdpSocket(NetProtocol protocol) :
	UdpSocket()
	{
		Create(protocol);
	}

	inline UdpSocket::UdpSocket(UdpSocket&& udpSocket) :
	AbstractSocket(std::move(udpSocket)),
	m_boundAddress(std::move(udpSocket.m_boundAddress)),
	m_state(udpSocket.m_state)
	{
	}

	inline SocketState UdpSocket::Bind(UInt16 port)
	{
		IpAddress any;
		switch (m_protocol)
		{
			case NetProtocol_Any:
				NazaraInternalError("Invalid protocol Any at this point");
				return SocketState_NotConnected;

			case NetProtocol_IPv4:
				any = IpAddress::AnyIpV4;
				break;

			case NetProtocol_IPv6:
				any = IpAddress::AnyIpV6;
				break;
		}

		any.SetPort(port);
		return Bind(any);
	}

	bool UdpSocket::Create(NetProtocol protocol)
	{
		return Open(protocol);
	}

	inline IpAddress UdpSocket::GetBoundAddress() const
	{
		return m_boundAddress;
	}

	inline UInt16 UdpSocket::GetBoundPort() const
	{
		return m_boundAddress.GetPort();
	}

	inline SocketState UdpSocket::GetState() const
	{
		return m_state;
	}
}

#include <Nazara/Network/DebugOff.hpp>

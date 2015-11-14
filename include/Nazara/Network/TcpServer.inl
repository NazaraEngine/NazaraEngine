// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline TcpServer::TcpServer() :
	AbstractSocket(SocketType_TCP)
	{
	}

	inline TcpServer::TcpServer(TcpServer&& tcpServer) :
	AbstractSocket(std::move(tcpServer)),
	m_boundAddress(std::move(tcpServer.m_boundAddress))
	{
	}

	inline IpAddress TcpServer::GetBoundAddress() const
	{
		return m_boundAddress;
	}

	inline UInt16 TcpServer::GetBoundPort() const
	{
		return m_boundAddress.GetPort();
	}

	inline SocketState TcpServer::Listen(NetProtocol protocol, UInt16 port, unsigned int queueSize)
	{
		NazaraAssert(protocol != NetProtocol_Any, "Any protocol not supported for Listen"); //< TODO
		NazaraAssert(protocol != NetProtocol_Unknown, "Invalid protocol"); 

		IpAddress any;
		switch (protocol)
		{
			case NetProtocol_Any:
			case NetProtocol_Unknown:
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
		return Listen(any, queueSize);
	}
}

#include <Nazara/Network/DebugOff.hpp>

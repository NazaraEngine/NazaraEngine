// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a TcpServer object by default
	*/

	inline TcpServer::TcpServer() :
	AbstractSocket(SocketType_TCP)
	{
	}

	/*!
	* \brief Constructs a TcpServer object with another one by move semantic
	*
	* \param tcpServer TcpServer to move into this
	*/

	inline TcpServer::TcpServer(TcpServer&& tcpServer) :
	AbstractSocket(std::move(tcpServer)),
	m_boundAddress(std::move(tcpServer.m_boundAddress))
	{
	}

	/*!
	* \brief Gets the bound address
	* \return IpAddress we are linked to
	*/

	inline IpAddress TcpServer::GetBoundAddress() const
	{
		return m_boundAddress;
	}

	/*!
	* \brief Gets the port of the bound address
	* \return Port we are linked to
	*/

	inline UInt16 TcpServer::GetBoundPort() const
	{
		return m_boundAddress.GetPort();
	}

	/*!
	* \brief Listens to a socket
	* \return State of the socket
	*
	* \param protocol Net protocol to listen to
	* \param port Port to listen to
	* \param queueSize Size of the queue
	*
	* \remark Produces a NazaraAssert if protocol is unknown or any
	*/

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

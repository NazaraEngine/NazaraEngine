// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a UdpSocket object by default
	*/

	inline UdpSocket::UdpSocket() :
	AbstractSocket(SocketType_UDP)
	{
	}

	/*!
	* \brief Constructs a UdpSocket object with a net protocol
	*
	* \param protocol Net protocol to use
	*/

	inline UdpSocket::UdpSocket(NetProtocol protocol) :
	UdpSocket()
	{
		Create(protocol);
	}

	/*!
	* \brief Constructs a UdpSocket object with another one by move semantic
	*
	* \param udpSocket UdpSocket to move into this
	*/

	inline UdpSocket::UdpSocket(UdpSocket&& udpSocket) :
	AbstractSocket(std::move(udpSocket)),
	m_boundAddress(std::move(udpSocket.m_boundAddress))
	{
	}

	/*!
	* \brief Binds a specific port
	* \return State of the socket
	*
	* \param port Port to bind
	*/

	inline SocketState UdpSocket::Bind(UInt16 port)
	{
		IpAddress any;
		switch (m_protocol)
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
		return Bind(any);
	}

	/*!
	* \brief Creates a UDP socket
	* \return true If successful
	*
	* \param protocol Net protocol to use
	*/

	bool UdpSocket::Create(NetProtocol protocol)
	{
		NazaraAssert(protocol != NetProtocol_Unknown, "Invalid protocol");

		return Open(protocol);
	}

	/*!
	* \brief Gets the bound address
	* \return IpAddress we are linked to
	*/

	inline IpAddress UdpSocket::GetBoundAddress() const
	{
		return m_boundAddress;
	}

	/*!
	* \brief Gets the port of the bound address
	* \return Port we are linked to
	*/

	inline UInt16 UdpSocket::GetBoundPort() const
	{
		return m_boundAddress.GetPort();
	}

	/*!
	* \brief Checks whether the broadcasting is enabled
	* \return true If it is the case
	*/

	inline bool UdpSocket::IsBroadcastingEnabled() const
	{
		return m_isBroadCastingEnabled;
	}
}

#include <Nazara/Network/DebugOff.hpp>

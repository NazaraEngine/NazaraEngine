// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/TcpServer.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Network/TcpClient.hpp>
#include <limits>
#include <Nazara/Network/Debug.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/SocketImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/SocketImpl.hpp>
#else
#error Missing implementation: Socket
#endif

namespace Nz
{
	/*!
	* \ingroup network
	* \class Nz::TcpServer
	* \brief Network class that represents a server in a TCP connection
	*/

	/*!
	* \brief Accepts a client
	* \return true If client'socket is valid
	*
	* \param newClient Client connection
	*
	* \remark Produces a NazaraAssert if socket is invalid
	* \remark Produces a NazaraAssert if newClient is invalid
	*/

	bool TcpServer::AcceptClient(TcpClient* newClient)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Server isn't listening");
		NazaraAssert(newClient, "Invalid client socket");

		IpAddress clientAddress;
		SocketHandle handle = SocketImpl::Accept(m_handle, &clientAddress, &m_lastError);
		if (handle != SocketImpl::InvalidHandle)
		{
			newClient->Reset(handle, clientAddress);
			return true;
		}
		else
			return false;
	}

	/*!
	* \brief Listens to a socket
	* \return State of the socket
	*
	* \param address Address to listen to
	* \param queueSize Size of the queue
	*
	* \remark Produces a NazaraAssert if address is invalid
	*/

	SocketState TcpServer::Listen(const IpAddress& address, unsigned int queueSize)
	{
		NazaraAssert(address.IsValid(), "Invalid address");

		Open(address.GetProtocol());

		SocketState state = SocketImpl::Listen(m_handle, address, queueSize, &m_lastError);
		if (state == SocketState_Bound)
			m_boundAddress = SocketImpl::QuerySocketAddress(m_handle);

		UpdateState(state);
		return state;
	}

	/*!
	* \brief Operation to do when closing socket
	*/

	void TcpServer::OnClose()
	{
		AbstractSocket::OnClose();

		m_boundAddress = IpAddress::Invalid;
	}

	/*!
	* \brief Operation to do when opening socket
	*/

	void TcpServer::OnOpened()
	{
		AbstractSocket::OnOpened();

		m_boundAddress = IpAddress::Invalid;
	}
}

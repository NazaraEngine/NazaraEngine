// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Network/UdpSocket.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>

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
	* \class Nz::UdpSocket
	* \brief Network class that represents a UDP socket, allowing for sending/receiving datagrams.
	*/

	/*!
	* \brief Binds a specific IpAddress
	* \return State of the socket
	*
	* \param address Address to bind
	*
	* \remark Produces a NazaraAssert if socket is invalid
	* \remark Produces a NazaraAssert if address is invalid
	*/

	SocketState UdpSocket::Bind(const IpAddress& address)
	{
		NazaraAssertMsg(m_handle != SocketImpl::InvalidHandle, "Socket hasn't been created");
		NazaraAssertMsg(address.IsValid(), "Invalid address");

		SocketState state = SocketImpl::Bind(m_handle, address, &m_lastError);
		if (state == SocketState::Bound)
			m_boundAddress = SocketImpl::QuerySocketAddress(m_handle);

		UpdateState(state);
		return state;
	}

	/*!
	* \brief Enables broadcasting
	*
	* \param broadcasting Should the UDP broadcast
	*
	* \remark Produces a NazaraAssert if socket is invalid
	*/

	void UdpSocket::EnableBroadcasting(bool broadcasting)
	{
		NazaraAssertMsg(m_handle != SocketImpl::InvalidHandle, "Invalid handle");

		if (m_isBroadCastingEnabled != broadcasting)
		{
			SocketImpl::SetBroadcasting(m_handle, broadcasting, &m_lastError);
			m_isBroadCastingEnabled = broadcasting;
		}
	}

	/*!
	* \brief Gets the maximum datagram size allowed
	* \return Number of bytes
	*
	* \remark Produces a NazaraAssert if socket is invalid
	*/

	std::size_t UdpSocket::QueryMaxDatagramSize()
	{
		NazaraAssertMsg(m_handle != SocketImpl::InvalidHandle, "Socket hasn't been created");

		return SocketImpl::QueryMaxDatagramSize(m_handle, &m_lastError);
	}

	/*!
	* \brief Receives the data available
	* \return true If data received
	*
	* \param buffer Raw memory to write
	* \param size Size of the buffer
	* \param from IpAddress of the peer
	* \param received Optional argument to get the number of bytes received
	*
	* \remark Produces a NazaraAssert if socket is invalid
	* \remark Produces a NazaraAssert if buffer and its size is invalid
	*/

	bool UdpSocket::Receive(void* buffer, std::size_t size, IpAddress* from, std::size_t* received)
	{
		NazaraAssertMsg(m_handle != SocketImpl::InvalidHandle, "Socket hasn't been created");
		NazaraAssertMsg(buffer && size > 0, "Invalid buffer");

		int read;
		if (!SocketImpl::ReceiveFrom(m_handle, buffer, static_cast<int>(size), from, &read, &m_lastError))
		{
			switch (m_lastError)
			{
				case SocketError::ConnectionClosed:
					m_lastError = SocketError::NoError;
					read = 0;
					break;

				default:
					return false;
			}
		}

		if (received)
			*received = read;

		return true;
	}

	/*!
	* \brief Receive multiple datagram from one peer
	* \return true If data were sent
	*
	* \param to Destination IpAddress (must match socket protocol)
	* \param buffers A pointer to an array of NetBuffer containing buffers and size data
	* \param bufferCount Number of buffers available
	* \param from IpAddress of the peer
	* \param received Optional argument to get the number of bytes received
	*/
	bool UdpSocket::ReceiveMultiple(NetBuffer* buffers, std::size_t bufferCount, IpAddress* from, std::size_t* received)
	{
		NazaraAssertMsg(m_handle != SocketImpl::InvalidHandle, "Socket hasn't been created");
		NazaraAssertMsg(buffers && bufferCount > 0, "Invalid buffer");

		int read;
		if (!SocketImpl::ReceiveMultiple(m_handle, buffers, bufferCount, from, &read, &m_lastError))
		{
			switch (m_lastError)
			{
				case SocketError::ConnectionClosed:
					m_lastError = SocketError::NoError;
					read = 0;
					break;

				default:
					return false;
			}
		}

		if (received)
			*received = read;

		return true;
	}

	/*!
	* \brief Sends the data available
	* \return true If data sended
	*
	* \param to IpAddress of the peer
	* \param buffer Raw memory to read
	* \param size Size of the buffer
	* \param sent Optional argument to get the number of bytes sent
	*
	* \remark Produces a NazaraAssert if peer address is invalid
	* \remark Produces a NazaraAssert if protocol of communication is not the same than the peer
	* \remark Produces a NazaraAssert if buffer and its size is invalid
	*/

	bool UdpSocket::Send(const IpAddress& to, const void* buffer, std::size_t size, std::size_t* sent)
	{
		NazaraAssertMsg(to.IsValid(), "Invalid ip address");
		NazaraAssertMsg(to.GetProtocol() == m_protocol, "IP Address has a different protocol than the socket");
		NazaraAssertMsg(buffer && size > 0, "Invalid buffer");

		int byteSent;
		if (!SocketImpl::SendTo(m_handle, buffer, static_cast<int>(size), to, &byteSent, &m_lastError))
			return false;

		if (sent)
			*sent = byteSent;

		return true;
	}

	/*!
	* \brief Sends multiple buffers as one datagram
	* \return true If data were sent
	*
	* \param to Destination IpAddress (must match socket protocol)
	* \param buffers A pointer to an array of NetBuffer containing buffers and size data
	* \param size Number of NetBuffer to send
	* \param sent Optional argument to get the number of bytes sent
	*/
	bool UdpSocket::SendMultiple(const IpAddress& to, const NetBuffer* buffers, std::size_t bufferCount, std::size_t* sent)
	{
		NazaraAssertMsg(to.IsValid(), "Invalid ip address");
		NazaraAssertMsg(to.GetProtocol() == m_protocol, "IP Address has a different protocol than the socket");
		NazaraAssertMsg(buffers && bufferCount > 0, "Invalid buffer");

		int byteSent;
		if (!SocketImpl::SendMultiple(m_handle, buffers, bufferCount, to, &byteSent, &m_lastError))
			return false;

		if (sent)
			*sent = byteSent;

		return true;
	}

	/*!
	* \brief Operation to do when closing socket
	*/

	void UdpSocket::OnClose()
	{
		AbstractSocket::OnClose();

		m_boundAddress = IpAddress::Invalid;
	}

	/*!
	* \brief Operation to do when opening socket
	*/

	void UdpSocket::OnOpened()
	{
		AbstractSocket::OnOpened();

		m_boundAddress = IpAddress::Invalid;
		m_isBroadCastingEnabled = false;
	}
}

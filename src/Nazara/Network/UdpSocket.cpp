// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/UdpSocket.hpp>
#include <Nazara/Network/NetPacket.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/SocketImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/SocketImpl.hpp>
#else
#error Missing implementation: Socket
#endif

#include <Nazara/Network/Debug.hpp>

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
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Socket hasn't been created");
		NazaraAssert(address.IsValid(), "Invalid address");

		SocketState state = SocketImpl::Bind(m_handle, address, &m_lastError);
		if (state == SocketState_Bound)
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
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");

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
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Socket hasn't been created");

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
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Socket hasn't been created");
		NazaraAssert(buffer && size > 0, "Invalid buffer");

		int read;
		if (!SocketImpl::ReceiveFrom(m_handle, buffer, static_cast<int>(size), from, &read, &m_lastError))
		{
			switch (m_lastError)
			{
				case SocketError_ConnectionClosed:
					m_lastError = SocketError_NoError;
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
	* \brief Receives the packet available
	* \return true If packet received
	*
	* \param packet Packet to receive
	* \param from IpAddress of the peer
	*
	* \remark Produces a NazaraAssert if packet is invalid
	* \remark Produces a NazaraWarning if packet's header is invalid
	*/

	bool UdpSocket::ReceivePacket(NetPacket* packet, IpAddress* from)
	{
		NazaraAssert(packet, "Invalid packet");

		// I'm not sure what's the best between having a 65k bytes buffer ready for any datagram size
		// or querying the next datagram size every time, for now I'll leave it as is
		packet->Reset(NetCode_Invalid, std::numeric_limits<UInt16>::max());
		packet->Resize(std::numeric_limits<UInt16>::max());

		std::size_t received;
		if (!Receive(packet->GetData(), static_cast<std::size_t>(packet->GetSize()), from, &received))
			return false;

		if (received == 0)
			return false; //< No datagram received

		Nz::UInt16 netCode;
		Nz::UInt16 packetSize;
		if (!NetPacket::DecodeHeader(packet->GetConstData(), &packetSize, &netCode))
		{
			m_lastError = SocketError_Packet;
			NazaraWarning("Invalid header data");
			return false;
		}

		if (packetSize != received)
		{
			m_lastError = SocketError_Packet;
			NazaraWarning("Invalid packet size (packet size is " + String::Number(packetSize) + " bytes, received " + Nz::String::Number(received) + " bytes)");
			return false;
		}

		packet->Resize(received);
		packet->SetNetCode(netCode);
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
		NazaraAssert(to.IsValid(), "Invalid ip address");
		NazaraAssert(to.GetProtocol() == m_protocol, "IP Address has a different protocol than the socket");
		NazaraAssert(buffer && size > 0, "Invalid buffer");

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
		NazaraAssert(to.IsValid(), "Invalid ip address");
		NazaraAssert(to.GetProtocol() == m_protocol, "IP Address has a different protocol than the socket");
		NazaraAssert(buffers && bufferCount > 0, "Invalid buffer");

		int byteSent;
		if (!SocketImpl::SendMultiple(m_handle, buffers, bufferCount, to, &byteSent, &m_lastError))
			return false;

		if (sent)
			*sent = byteSent;

		return true;
	}

	/*!
	* \brief Sends the packet available
	* \return true If packet sent
	*
	* \param to IpAddress of the peer
	* \param packet Packet to send
	*
	* \remark Produces a NazaraError if packet could not be prepared for sending
	*/

	bool UdpSocket::SendPacket(const IpAddress& to, const NetPacket& packet)
	{
		std::size_t size = 0;
		const UInt8* ptr = static_cast<const UInt8*>(packet.OnSend(&size));
		if (!ptr)
		{
			m_lastError = SocketError_Packet;
			NazaraError("Failed to prepare packet");
			return false;
		}

		return Send(to, ptr, size, nullptr);
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

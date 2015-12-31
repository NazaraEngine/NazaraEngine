// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/UdpSocket.hpp>
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

	void UdpSocket::EnableBroadcasting(bool broadcasting)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");

		if (m_isBroadCastingEnabled != broadcasting)
		{
			SocketImpl::SetBroadcasting(m_handle, broadcasting, &m_lastError);
			m_isBroadCastingEnabled = broadcasting;
		}
	}

	unsigned int UdpSocket::QueryMaxDatagramSize()
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Socket hasn't been created");

		return SocketImpl::QueryMaxDatagramSize(m_handle, &m_lastError);
	}

	bool UdpSocket::Receive(void* buffer, std::size_t size, IpAddress* from, std::size_t* received)
	{
		NazaraAssert(buffer && size > 0, "Invalid buffer");

		int read;
		if (!SocketImpl::ReceiveFrom(m_handle, buffer, static_cast<int>(size), from, &read, &m_lastError))
			return false;

		if (received)
			*received = read;

		return true;
	}

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

	void UdpSocket::OnClose()
	{
		AbstractSocket::OnClose();

		m_boundAddress = IpAddress::Invalid;
	}

	void UdpSocket::OnOpened()
	{
		AbstractSocket::OnOpened();

		m_boundAddress = IpAddress::Invalid;
		m_isBroadCastingEnabled = false;
	}
}

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/TcpClient.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
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
	SocketState TcpClient::Connect(const IpAddress& remoteAddress)
	{
		NazaraAssert(remoteAddress.IsValid(), "Invalid remote address");
		NazaraAssert(remoteAddress.GetPort() != 0, "Remote address has no port");

		Open(remoteAddress.GetProtocol());

		CallOnExit restoreBlocking;
		if (m_isBlockingEnabled)
		{
			SocketImpl::SetBlocking(m_handle, false);
			restoreBlocking.Reset([this] ()
			{
				SocketImpl::SetBlocking(m_handle, true);
			});
		}

		SocketState state = SocketImpl::Connect(m_handle, remoteAddress, &m_lastError);
		m_peerAddress = (state != SocketState_NotConnected) ? remoteAddress : IpAddress::Invalid;

		UpdateState(state);
		return state;
	}

	SocketState TcpClient::Connect(const String& hostName, NetProtocol protocol, const String& service, ResolveError* error)
	{
		Disconnect();

		UpdateState(SocketState_Resolving);
		std::vector<HostnameInfo> results = IpAddress::ResolveHostname(protocol, hostName, service, error);
		if (results.empty())
		{
			m_lastError = SocketError_ResolveError;

			UpdateState(SocketState_NotConnected);
			return m_state;
		}

		IpAddress hostnameAddress;
		for (const HostnameInfo& result : results)
		{
			if (!result.address)
				continue;

			if (result.socketType != SocketType_TCP)
				continue;

			hostnameAddress = result.address;
			break; //< Take first valid address
		}

		return Connect(hostnameAddress);
	}

	void TcpClient::EnableLowDelay(bool lowDelay)
	{
		if (m_isLowDelayEnabled != lowDelay)
		{
			if (m_handle != SocketImpl::InvalidHandle)
				SocketImpl::SetNoDelay(m_handle, lowDelay, &m_lastError);

			m_isLowDelayEnabled = lowDelay;
		}
	}

	void TcpClient::EnableKeepAlive(bool keepAlive, UInt64 msTime, UInt64 msInterval)
	{
		if (m_isKeepAliveEnabled != keepAlive || m_keepAliveTime != msTime || m_keepAliveInterval != msInterval)
		{
			if (m_handle != SocketImpl::InvalidHandle)
				SocketImpl::SetKeepAlive(m_handle, keepAlive, msTime, msInterval, &m_lastError);

			m_isKeepAliveEnabled = keepAlive;
			m_keepAliveInterval = msInterval;
			m_keepAliveTime = msTime;
		}
	}

	bool TcpClient::EndOfStream() const
	{
		return QueryAvailableBytes() == 0;
	}

	UInt64 TcpClient::GetCursorPos() const
	{
		NazaraError("GetCursorPos() cannot be used on sequential streams");
		return 0;
	}

	UInt64 TcpClient::GetSize() const
	{
		return QueryAvailableBytes();
	}

	bool TcpClient::Receive(void* buffer, std::size_t size, std::size_t* received)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");
		NazaraAssert(buffer && size > 0, "Invalid buffer");

		int read;
		if (!SocketImpl::Receive(m_handle, buffer, static_cast<int>(size), &read, &m_lastError))
		{
			switch (m_lastError)
			{
				case SocketError_ConnectionClosed:
				case SocketError_ConnectionRefused:
					UpdateState(SocketState_NotConnected);
					break;

				default:
					break;
			}

			return false;
		}

		if (received)
			*received = read;

		UpdateState(SocketState_Connected);
		return true;
	}

	bool TcpClient::Send(const void* buffer, std::size_t size, std::size_t* sent)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");
		NazaraAssert(buffer && size > 0, "Invalid buffer");

		CallOnExit updateSent;
		std::size_t totalByteSent = 0;
		if (sent)
		{
			updateSent.Reset([sent, &totalByteSent] ()
			{
				*sent = totalByteSent;
			});
		}

		while (totalByteSent < size)
		{
			int sendSize = static_cast<int>(std::min<std::size_t>(size - totalByteSent, std::numeric_limits<int>::max())); //< Handle very large send
			int sentSize;
			if (!SocketImpl::Send(m_handle, reinterpret_cast<const UInt8*>(buffer) + totalByteSent, sendSize, &sentSize, &m_lastError))
			{
				switch (m_lastError)
				{
					case SocketError_ConnectionClosed:
					case SocketError_ConnectionRefused:
						UpdateState(SocketState_NotConnected);
						break;

					default:
						break;
				}

				return false;
			}

			totalByteSent += sentSize;
		}

		UpdateState(SocketState_Connected);
		return true;
	}

	bool TcpClient::SetCursorPos(UInt64 offset)
	{
		NazaraError("SetCursorPos() cannot be used on sequential streams");
		return false;
	}

	bool TcpClient::WaitForConnected(UInt64 msTimeout)
	{
		switch (m_state)
		{
			case SocketState_Bound:
			case SocketState_Resolving:
				break;

			case SocketState_Connected:
				return true;

			case SocketState_Connecting:
			{
				NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");

				CallOnExit restoreBlocking;
				if (m_isBlockingEnabled)
				{
					SocketImpl::SetBlocking(m_handle, false);
					restoreBlocking.Reset([this] ()
					{
						SocketImpl::SetBlocking(m_handle, true);
					});
				}

				SocketState newState = SocketImpl::Connect(m_handle, m_peerAddress, msTimeout, &m_lastError);
				NazaraAssert(newState != SocketState_Connecting, "Invalid internal return");

				// Prevent valid peer address in non-connected state
				if (newState == SocketState_NotConnected)
					m_peerAddress = IpAddress::Invalid;

				UpdateState(newState);
				return newState == SocketState_Connected;
			}

			case SocketState_NotConnected:
				return false;
		}

		NazaraInternalError("Unhandled socket state (0x" + String::Number(m_state, 16) + ')');
		return false;
	}

	void TcpClient::FlushStream()
	{
	}

	void TcpClient::OnClose()
	{
		AbstractSocket::OnClose();

		m_openMode = OpenMode_NotOpen;
		m_peerAddress = IpAddress::Invalid;
	}

	void TcpClient::OnOpened()
	{
		AbstractSocket::OnOpened();

		SocketError errorCode;

		if (!SocketImpl::SetNoDelay(m_handle, m_isLowDelayEnabled, &errorCode))
			NazaraWarning("Failed to set socket no delay mode (0x" + String::Number(errorCode, 16) + ')');

		if (!SocketImpl::SetKeepAlive(m_handle, m_isKeepAliveEnabled, m_keepAliveTime, m_keepAliveInterval, &errorCode))
			NazaraWarning("Failed to set socket keep alive mode (0x" + String::Number(errorCode, 16) + ')');

		m_peerAddress = IpAddress::Invalid;
		m_openMode = OpenMode_ReadWrite;
	}

	std::size_t TcpClient::ReadBlock(void* buffer, std::size_t size)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");

		CallOnExit restoreBlocking;
		if (!m_isBlockingEnabled)
		{
			SocketImpl::SetBlocking(m_handle, true);
			restoreBlocking.Reset([this] ()
			{
				SocketImpl::SetBlocking(m_handle, false);
			});
		}

		std::size_t received;
		if (!Receive(buffer, size, &received))
			received = 0;

		return received;
	}

	void TcpClient::Reset(SocketHandle handle, const IpAddress& peerAddress)
	{
		Open(handle);
		m_peerAddress = peerAddress;
		m_openMode = OpenMode_ReadWrite;
		UpdateState(SocketState_Connected);
	}

	std::size_t TcpClient::WriteBlock(const void* buffer, std::size_t size)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");

		CallOnExit restoreBlocking;
		if (!m_isBlockingEnabled)
		{
			SocketImpl::SetBlocking(m_handle, true);
			restoreBlocking.Reset([this] ()
			{
				SocketImpl::SetBlocking(m_handle, false);
			});
		}

		std::size_t sent;
		if (!Send(buffer, size, &sent))
			sent = 0;

		return sent;
	}
}

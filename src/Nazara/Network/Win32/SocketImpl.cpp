// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Win32/SocketImpl.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Network/Win32/IpAddressImpl.hpp>

// Some compilers (older versions of MinGW) lack Mstcpip.h which defines some structs/defines
#if defined(__has_include)
	#define NZ_HAS_MSTCPIP_HEADER __has_include(<mstcpip.h>)
#else
	// If this version of MinGW doesn't support __has_include, assume it hasn't Mstcpip.h
	#define NZ_HAS_MSTCPIP_HEADER !defined(NAZARA_COMPILER_MINGW)
#endif

#if NZ_HAS_MSTCPIP_HEADER
#include <mstcpip.h>
#else
struct tcp_keepalive
{
	u_long onoff;
	u_long keepalivetime;
	u_long keepaliveinterval;
};

#define SIO_KEEPALIVE_VALS    _WSAIOW(IOC_VENDOR,4)
#endif

#include <winsock2.h>

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	SocketHandle SocketImpl::Accept(SocketHandle handle, IpAddress* address, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		std::fill(nameBuffer.begin(), nameBuffer.end(), 0);

		int bufferLength = static_cast<int>(nameBuffer.size());

		SocketHandle newClient = accept(handle, reinterpret_cast<sockaddr*>(&nameBuffer), &bufferLength);
		if (newClient != InvalidHandle)
		{
			if (address)
				*address = IpAddressImpl::FromSockAddr(reinterpret_cast<const sockaddr*>(&nameBuffer));

			if (error)
				*error = SocketError::NoError;
		}
		else
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());
		}

		return newClient;
	}

	SocketState SocketImpl::Bind(SocketHandle handle, const IpAddress& address, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(address.IsValid(), "Invalid address");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		int bufferLength = IpAddressImpl::ToSockAddr(address, nameBuffer.data());

		if (bind(handle, reinterpret_cast<const sockaddr*>(&nameBuffer), bufferLength) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return SocketState::NotConnected;
		}

		if (error)
			*error = SocketError::NoError;

		return SocketState::Bound;
	}

	SocketHandle SocketImpl::Create(NetProtocol protocol, SocketType type, SocketError* error)
	{
		NazaraAssert(protocol != NetProtocol::Any, "Any protocol is not supported for socket creation");
		NazaraAssert(type <= SocketType::Max, "Type has value out of enum");

		SocketHandle handle = socket(TranslateNetProtocolToAF(protocol), TranslateSocketTypeToSock(type), 0);
		if (handle == InvalidHandle && error != nullptr)
			*error = TranslateWSAErrorToSocketError(WSAGetLastError());

		return handle;
	}

	void SocketImpl::Close(SocketHandle handle)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		if (closesocket(handle) == SOCKET_ERROR)
			NazaraWarning("Failed to close socket: " + Error::GetLastSystemError(WSAGetLastError()));
	}

	void SocketImpl::ClearErrorCode(SocketHandle handle)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		if (GetLastError(handle, nullptr) == SocketError::Internal)
			NazaraWarning("Failed to clear socket error code: " + Error::GetLastSystemError(WSAGetLastError()));
	}

	SocketState SocketImpl::Connect(SocketHandle handle, const IpAddress& address, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(address.IsValid(), "Invalid address");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		int bufferLength = IpAddressImpl::ToSockAddr(address, nameBuffer.data());

		if (error)
			*error = SocketError::NoError;

		// Clear socket error status
		ClearErrorCode(handle);

		if (connect(handle, reinterpret_cast<const sockaddr*>(nameBuffer.data()), bufferLength) == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			switch (errorCode) //< Check for "normal errors" first
			{
				case WSAEALREADY:
				case WSAEINVAL: //< In case of connect, WSAEINVAL may be returned instead of WSAEALREADY
				case WSAEWOULDBLOCK:
					return SocketState::Connecting;

				case WSAEISCONN:
					return SocketState::Connected;
			}

			if (error)
			{
				if (errorCode == WSAEADDRNOTAVAIL)
					*error = SocketError::ConnectionRefused; //< ConnectionRefused seems more legit than AddressNotAvailable in connect case
				else
					*error = TranslateWSAErrorToSocketError(errorCode);
			}

			return SocketState::NotConnected;
		}

		return SocketState::Connected;
	}

	bool SocketImpl::Initialize()
	{
		int errorCode = WSAStartup(MAKEWORD(2, 2), &s_WSA);
		if (errorCode != 0)
		{
			NazaraError("Failed to initialize Windows Socket 2.2: " + Error::GetLastSystemError(errorCode));
			return false;
		}

		NazaraDebug("Initialized Windows Socket " + NumberToString(LOBYTE(s_WSA.wVersion)) + '.' + NumberToString(HIBYTE(s_WSA.wVersion)) + " (" + std::string(s_WSA.szDescription) + ')');
		return true;
	}

	SocketError SocketImpl::GetLastError(SocketHandle handle, SocketError* error)
	{
		int code = GetLastErrorCode(handle, error);
		if (code < 0)
			return SocketError::Internal;

		return TranslateWSAErrorToSocketError(code);
	}

	int SocketImpl::GetLastErrorCode()
	{
		return WSAGetLastError();
	}

	int SocketImpl::GetLastErrorCode(SocketHandle handle, SocketError* error)
	{
		int code;
		int codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&code), &codeLength) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return -1;
		}

		if (error)
			*error = SocketError::NoError;

		return code;
	}

	SocketState SocketImpl::Listen(SocketHandle handle, const IpAddress& address, unsigned queueSize, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(address.IsValid(), "Invalid address");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		int bufferLength = IpAddressImpl::ToSockAddr(address, nameBuffer.data());

		if (bind(handle, reinterpret_cast<const sockaddr*>(&nameBuffer), bufferLength) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return SocketState::NotConnected;
		}

		if (listen(handle, queueSize) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return SocketState::NotConnected;
		}

		if (error)
			*error = SocketError::NoError;

		return SocketState::Bound;
	}

	std::size_t SocketImpl::QueryAvailableBytes(SocketHandle handle, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		u_long availableBytes;
		if (ioctlsocket(handle, FIONREAD, &availableBytes) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return 0;
		}

		if (error)
			*error = SocketError::NoError;

		return availableBytes;
	}

	bool SocketImpl::QueryBroadcasting(SocketHandle handle, SocketError* error)
	{
		BOOL code;
		int codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&code), &codeLength) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return false;
		}

		if (error)
			*error = SocketError::NoError;

		return code == TRUE;
	}

	bool SocketImpl::QueryKeepAlive(SocketHandle handle, SocketError* error)
	{
		BOOL code;
		int codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&code), &codeLength) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return false;
		}

		if (error)
			*error = SocketError::NoError;

		return code == TRUE;
	}

	std::size_t SocketImpl::QueryMaxDatagramSize(SocketHandle handle, SocketError* error)
	{
		DWORD code;
		int codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_MAX_MSG_SIZE, reinterpret_cast<char*>(&code), &codeLength) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return 0;
		}

		if (error)
			*error = SocketError::NoError;

		return code;
	}

	bool SocketImpl::QueryNoDelay(SocketHandle handle, SocketError* error)
	{
		BOOL code;
		int codeLength = sizeof(code);

		if (getsockopt(handle, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&code), &codeLength) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return false;
		}

		if (error)
			*error = SocketError::NoError;

		return code == TRUE;
	}

	std::size_t SocketImpl::QueryReceiveBufferSize(SocketHandle handle, SocketError* error)
	{
		DWORD code;
		int codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&code), &codeLength) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return 0;
		}

		if (error)
			*error = SocketError::NoError;

		return code;
	}

	IpAddress SocketImpl::QueryPeerAddress(SocketHandle handle, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		std::fill(nameBuffer.begin(), nameBuffer.end(), 0);

		int bufferLength = static_cast<int>(nameBuffer.size());

		if (getpeername(handle, reinterpret_cast<sockaddr*>(nameBuffer.data()), &bufferLength) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return IpAddress();
		}

		if (error)
			*error = SocketError::NoError;

		return IpAddressImpl::FromSockAddr(reinterpret_cast<sockaddr*>(nameBuffer.data()));
	}

	IpAddress SocketImpl::QuerySocketAddress(SocketHandle handle, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		std::fill(nameBuffer.begin(), nameBuffer.end(), 0);

		int bufferLength = static_cast<int>(nameBuffer.size());

		if (getsockname(handle, reinterpret_cast<sockaddr*>(nameBuffer.data()), &bufferLength) == SOCKET_ERROR)
		{
			if (error)
			{
				int errorCode = WSAGetLastError();
				if (errorCode == WSAEINVAL)
					*error = SocketError::NoError;
				else
					*error = TranslateWSAErrorToSocketError(errorCode);
			}

			return IpAddress();
		}

		if (error)
			*error = SocketError::NoError;

		return IpAddressImpl::FromSockAddr(reinterpret_cast<sockaddr*>(nameBuffer.data()));
	}

	std::size_t SocketImpl::QuerySendBufferSize(SocketHandle handle, SocketError* error)
	{
		DWORD code;
		int codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&code), &codeLength) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return 0;
		}

		if (error)
			*error = SocketError::NoError;

		return code;
	}

	unsigned int SocketImpl::Poll(PollSocket* fdarray, std::size_t nfds, int timeout, SocketError* error)
	{
		NazaraAssert(fdarray && nfds > 0, "Invalid fdarray");

		#if NAZARA_NETWORK_POLL_SUPPORT
		static_assert(sizeof(PollSocket) == sizeof(WSAPOLLFD), "PollSocket size must match WSAPOLLFD size");

		int result = WSAPoll(reinterpret_cast<WSAPOLLFD*>(fdarray), static_cast<ULONG>(nfds), timeout);
		if (result == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			if (error)
				*error = TranslateWSAErrorToSocketError(errorCode);

			return 0;
		}

		assert(result >= 0);

		if (error)
			*error = SocketError::NoError;

		return static_cast<unsigned int>(result);
		#else
		NazaraUnused(fdarray);
		NazaraUnused(nfds);
		NazaraUnused(timeout);

		if (error)
			*error = SocketError::NotSupported;

		return 0;
		#endif
	}
	
	SocketState SocketImpl::PollConnection(SocketHandle handle, const IpAddress& /*address*/, UInt64 msTimeout, SocketError* error)
	{
		// Wait until socket is available for writing or an error occurs (ie when connection succeeds or fails)
		WSAPOLLFD descriptor;
		descriptor.events = POLLWRNORM;
		descriptor.fd = handle;
		descriptor.revents = 0;

		int ret = WSAPoll(&descriptor, 1, (msTimeout != std::numeric_limits<UInt64>::max()) ? INT(msTimeout) : INT(-1));
		if (ret == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return SocketState::NotConnected;
		}
		else if (ret > 0)
		{
			if (descriptor.revents & (POLLERR | POLLHUP))
			{
				if (error)
					*error = GetLastError(handle);

				return SocketState::NotConnected;
			}
			else if (descriptor.revents & POLLWRNORM)
				return SocketState::Connected;
			else
			{
				NazaraWarning("Socket " + std::to_string(handle) + " was returned by poll without POLLOUT nor error events (events: 0x" + NumberToString(descriptor.revents, 16) + ')');
				return SocketState::NotConnected;
			}
		}
		else
		{
			// Still connecting
			if (error)
			{
				if (msTimeout > 0)
					*error = SocketError::TimedOut;
				else
					*error = SocketError::NoError;
			}

			return SocketState::Connecting;
		}
	}

	bool SocketImpl::Receive(SocketHandle handle, void* buffer, int length, int* read, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(buffer && length > 0, "Invalid buffer");

		int byteRead = recv(handle, static_cast<char*>(buffer), length, 0);
		if (byteRead == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			switch (errorCode)
			{
				case WSAEWOULDBLOCK:
				{
					// If we have no data and are not blocking, return true with 0 byte read
					byteRead = 0;
					break;
				}

				default:
				{
					if (error)
						*error = TranslateWSAErrorToSocketError(errorCode);

					return false; //< Error
				}
			}
		}
		else if (byteRead == 0)
		{
			if (error)
				*error = SocketError::ConnectionClosed;

			return false; //< Connection has been closed
		}

		if (read)
			*read = byteRead;

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::ReceiveFrom(SocketHandle handle, void* buffer, int length, IpAddress* from, int* read, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(buffer && length > 0, "Invalid buffer");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		std::fill(nameBuffer.begin(), nameBuffer.end(), 0);

		int bufferLength = static_cast<int>(nameBuffer.size());

		IpAddress senderIp;

		int byteRead = recvfrom(handle, reinterpret_cast<char*>(buffer), length, 0, reinterpret_cast<sockaddr*>(&nameBuffer), &bufferLength);
		if (byteRead == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			switch (errorCode)
			{
				case WSAEWOULDBLOCK:
				{
					// If we have no data and are not blocking, return true with 0 byte read
					byteRead = 0;
					senderIp = IpAddress::Invalid;
					break;
				}

				default:
				{
					if (error)
						*error = TranslateWSAErrorToSocketError(errorCode);

					return false; //< Error
				}
			}
		}
		else if (byteRead == 0)
		{
			if (error)
				*error = SocketError::ConnectionClosed;

			return false; //< Connection closed
		}
		else // else we received something
			senderIp = IpAddressImpl::FromSockAddr(reinterpret_cast<const sockaddr*>(&nameBuffer));

		if (from)
			*from = senderIp;

		if (read)
			*read = byteRead;

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::ReceiveMultiple(SocketHandle handle, NetBuffer* buffers, std::size_t bufferCount, IpAddress* from, int* read, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(buffers && bufferCount > 0, "Invalid buffers");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		std::fill(nameBuffer.begin(), nameBuffer.end(), 0);

		int bufferLength = static_cast<int>(nameBuffer.size());

		IpAddress senderIp;

		StackArray<WSABUF> winBuffers = NazaraStackArray(WSABUF, bufferCount);
		for (std::size_t i = 0; i < bufferCount; ++i)
		{
			winBuffers[i].buf = static_cast<CHAR*>(buffers[i].data);
			winBuffers[i].len = static_cast<ULONG>(buffers[i].dataLength);
		}

		DWORD flags = 0;
		DWORD byteRead;
		if (WSARecvFrom(handle, winBuffers.data(), static_cast<DWORD>(bufferCount), &byteRead, &flags, reinterpret_cast<sockaddr*>(nameBuffer.data()), &bufferLength, nullptr, nullptr) == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			switch (errorCode)
			{
				case WSAECONNRESET:
				case WSAEWOULDBLOCK:
				{
					// If we have no data and are not blocking, return true with 0 byte read
					byteRead = 0;
					senderIp = IpAddress::Invalid;
					break;
				}

				default:
				{
					if (error)
						*error = TranslateWSAErrorToSocketError(errorCode);

					return false; //< Error
				}
			}
		}
		else
			senderIp = IpAddressImpl::FromSockAddr(reinterpret_cast<const sockaddr*>(&nameBuffer));

		if (flags & MSG_PARTIAL)
		{
			if (error)
				*error = SocketError::DatagramSize;

			return false;
		}

		if (from)
			*from = senderIp;

		if (read)
			*read = byteRead;

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::Send(SocketHandle handle, const void* buffer, int length, int* sent, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(buffer && length > 0, "Invalid buffer");

		int byteSent = send(handle, reinterpret_cast<const char*>(buffer), length, 0);
		if (byteSent == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			switch (errorCode)
			{
				case WSAEWOULDBLOCK:
				{
					byteSent = 0;
					break;
				}

				default:
				{
					if (error)
						*error = TranslateWSAErrorToSocketError(errorCode);

					return false; //< Error
				}
			}
		}

		if (sent)
			*sent = byteSent;

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SendMultiple(SocketHandle handle, const NetBuffer* buffers, std::size_t bufferCount, const IpAddress& to, int* sent, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(buffers && bufferCount > 0, "Invalid buffers");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		int bufferLength = IpAddressImpl::ToSockAddr(to, nameBuffer.data());

		StackArray<WSABUF> winBuffers = NazaraStackArray(WSABUF, bufferCount);
		for (std::size_t i = 0; i < bufferCount; ++i)
		{
			winBuffers[i].buf = static_cast<CHAR*>(buffers[i].data);
			winBuffers[i].len = static_cast<ULONG>(buffers[i].dataLength);
		}

		DWORD byteSent;
		if (WSASendTo(handle, winBuffers.data(), static_cast<DWORD>(bufferCount), &byteSent, 0, reinterpret_cast<const sockaddr*>(nameBuffer.data()), bufferLength, nullptr, nullptr) == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			switch (errorCode)
			{
				case WSAEWOULDBLOCK:
				{
					byteSent = 0;
					break;
				}

				default:
				{
					if (error)
						*error = TranslateWSAErrorToSocketError(errorCode);

					return false; //< Error
				}
			}
		}

		if (sent)
			*sent = static_cast<int>(byteSent);

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SendTo(SocketHandle handle, const void* buffer, int length, const IpAddress& to, int* sent, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(buffer && length > 0, "Invalid buffer");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		int bufferLength = IpAddressImpl::ToSockAddr(to, nameBuffer.data());

		int byteSent = sendto(handle, reinterpret_cast<const char*>(buffer), length, 0, reinterpret_cast<const sockaddr*>(nameBuffer.data()), bufferLength);
		if (byteSent == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			switch (errorCode)
			{
				case WSAEWOULDBLOCK:
				{
					byteSent = 0;
					break;
				}

				default:
				{
					if (error)
						*error = TranslateWSAErrorToSocketError(errorCode);

					return false; //< Error
				}
			}
		}

		if (sent)
			*sent = byteSent;

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetBlocking(SocketHandle handle, bool blocking, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		u_long block = (blocking) ? 0 : 1;
		if (ioctlsocket(handle, FIONBIO, &block) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetBroadcasting(SocketHandle handle, bool broadcasting, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		BOOL option = broadcasting;
		if (setsockopt(handle, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&option), sizeof(option)) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetIPv6Only(SocketHandle handle, bool ipv6Only, SocketError* error)
	{
#if NAZARA_CORE_WINDOWS_NT6
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		DWORD option = ipv6Only;
		if (setsockopt(handle, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&option), sizeof(option)) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
#else
		if (error)
			*error = SocketError::NotSupported;

		return false;
#endif
	}

	bool SocketImpl::SetKeepAlive(SocketHandle handle, bool enabled, UInt64 msTime, UInt64 msInterval, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		tcp_keepalive keepAlive;
		keepAlive.onoff = (enabled) ? 1 : 0;
		keepAlive.keepaliveinterval = static_cast<ULONG>(msInterval);
		keepAlive.keepalivetime = static_cast<ULONG>(msTime);

		DWORD dummy; //< byteReturned
		if (WSAIoctl(handle, SIO_KEEPALIVE_VALS, &keepAlive, sizeof(keepAlive), nullptr, 0, &dummy, nullptr, nullptr) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetNoDelay(SocketHandle handle, bool nodelay, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		BOOL option = nodelay;
		if (setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&option), sizeof(option)) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetReceiveBufferSize(SocketHandle handle, std::size_t size, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		DWORD option = static_cast<DWORD>(size);
		if (setsockopt(handle, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&option), sizeof(option)) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetSendBufferSize(SocketHandle handle, std::size_t size, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		DWORD option = static_cast<DWORD>(size);
		if (setsockopt(handle, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&option), sizeof(option)) == SOCKET_ERROR)
		{
			if (error)
				*error = TranslateWSAErrorToSocketError(WSAGetLastError());

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	SocketError SocketImpl::TranslateWSAErrorToSocketError(int error)
	{
		switch (error)
		{
			case 0:
				return SocketError::NoError;

			// Engine error
			case WSAEACCES:
			case WSAEBADF:
			case WSAEINVAL:
			case WSAEFAULT:
			case WSAENOTSOCK:
			case WSAEPROTOTYPE:
			case WSA_INVALID_HANDLE:
			// Those are not errors and should have been handled
			case WSAEALREADY:
			case WSAEISCONN:
			case WSAEWOULDBLOCK:
				NazaraWarning("Internal error occurred: " + Error::GetLastSystemError(error) + " (" + NumberToString(error) + ')');
				return SocketError::Internal;

			case WSAEADDRNOTAVAIL:
			case WSAEADDRINUSE:
				return SocketError::AddressNotAvailable;

			case WSAEAFNOSUPPORT:
			case WSAEPFNOSUPPORT:
			case WSAEOPNOTSUPP:
			case WSAEPROTONOSUPPORT:
			case WSAESOCKTNOSUPPORT:
				return SocketError::NotSupported;

			case WSAECONNREFUSED:
				return SocketError::ConnectionRefused;

			case WSAECONNABORTED:
			case WSAECONNRESET:
			case WSAENOTCONN:
			case WSAESHUTDOWN:
				return SocketError::ConnectionClosed;

			case WSAEMSGSIZE:
				return SocketError::DatagramSize;

			case WSAEMFILE:
			case WSAENOBUFS:
			case WSA_NOT_ENOUGH_MEMORY:
				return SocketError::ResourceError;

			case WSAEHOSTUNREACH:
				return SocketError::UnreachableHost;

			case WSAENETDOWN:
			case WSAENETUNREACH:
				return SocketError::NetworkError;

			case WSANOTINITIALISED:
				return SocketError::NotInitialized;

			case WSAETIMEDOUT:
				return SocketError::TimedOut;
		}

		NazaraWarning("Unhandled WinSock error: " + Error::GetLastSystemError(error) + " (" + NumberToString(error) + ')');
		return SocketError::Unknown;
	}

	int SocketImpl::TranslateNetProtocolToAF(NetProtocol protocol)
	{
		NazaraAssert(protocol <= NetProtocol::Max, "Protocol has value out of enum");

		static int addressFamily[] = {
			AF_UNSPEC, //< NetProtocol::Any
			AF_INET,   //< NetProtocol::IPv4
			AF_INET6,  //< NetProtocol::IPv6
			-1         //< NetProtocol::Unknown
		};
		static_assert(sizeof(addressFamily) / sizeof(int) == NetProtocolCount, "Address family array is incomplete");

		return addressFamily[UnderlyingCast(protocol)];
	}

	int SocketImpl::TranslateSocketTypeToSock(SocketType type)
	{
		NazaraAssert(type <= SocketType::Max, "Socket type has value out of enum");

		static int socketType[] = {
			SOCK_RAW,     //< SocketType::Raw
			SOCK_STREAM,  //< SocketType::TCP
			SOCK_DGRAM,   //< SocketType::UDP
			-1            //< SocketType::Unknown
		};
		static_assert(sizeof(socketType) / sizeof(int) == SocketTypeCount, "Socket type array is incomplete");

		return socketType[UnderlyingCast(type)];
	}

	void SocketImpl::Uninitialize()
	{
		WSACleanup();
	}

	SocketHandle SocketImpl::InvalidHandle = INVALID_SOCKET;
	WSADATA SocketImpl::s_WSA;
}

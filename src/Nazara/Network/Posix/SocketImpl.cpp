// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Posix/SocketImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Network/Algorithm.hpp>
#include <Nazara/Network/NetBuffer.hpp>
#include <Nazara/Network/Posix/IpAddressImpl.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <cstring>
#include <poll.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <Nazara/Network/Debug.hpp>

#if !defined(TCP_KEEPIDLE) && defined(TCP_KEEPALIVE)
#define TCP_KEEPIDLE TCP_KEEPALIVE // see -> https://gitlab.freedesktop.org/spice/usbredir/-/issues/9
#endif

namespace Nz
{
	SocketHandle SocketImpl::Accept(SocketHandle handle, IpAddress* address, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		nameBuffer.fill(0);

		socklen_t bufferLength = sizeof(sockaddr_in);

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
				*error = TranslateErrorToSocketError(errno);
		}

		return newClient;
	}

	SocketState SocketImpl::Bind(SocketHandle handle, const IpAddress& address, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(address.IsValid(), "Invalid address");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		int bufferLength = IpAddressImpl::ToSockAddr(address, nameBuffer.data());

		if (bind(handle, reinterpret_cast<const sockaddr*>(&nameBuffer), bufferLength) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

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
			*error = TranslateErrorToSocketError(errno);

		return handle;
	}

	void SocketImpl::Close(SocketHandle handle)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		if (close(handle) == -1)
			NazaraWarningFmt("failed to close socket: {0}", Error::GetLastSystemError(errno));
	}

	void SocketImpl::ClearErrorCode(SocketHandle handle)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		SocketError error;
		if (GetLastError(handle, &error) != SocketError::NoError)
			NazaraWarningFmt("failed to clear socket error code: {0}", ErrorToString(error));
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

		if (connect(handle, reinterpret_cast<const sockaddr*>(nameBuffer.data()), bufferLength) == -1)
		{
			int errorCode = errno;
			switch (errorCode) //< Check for "normal errors" first
			{
				case EALREADY:
				case EINPROGRESS:
					return SocketState::Connecting;

				case EISCONN:
					return SocketState::Connected;
			}

			if (error)
			{
				if (errorCode == EADDRNOTAVAIL)
					*error = SocketError::ConnectionRefused; //< ConnectionRefused seems more legit than AddressNotAvailable in connect case
				else
					*error = TranslateErrorToSocketError(errorCode);
			}

			return SocketState::NotConnected;
		}

		return SocketState::Connected;
	}

	bool SocketImpl::Initialize()
	{
		return true;
	}

	SocketError SocketImpl::GetLastError(SocketHandle handle, SocketError* error)
	{
		int code = GetLastErrorCode(handle, error);
		if (code < 0)
			return SocketError::Internal;

		return TranslateErrorToSocketError(code);
	}

	int SocketImpl::GetLastErrorCode(SocketHandle handle, SocketError* error)
	{
		int code;
		socklen_t codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_ERROR, &code, &codeLength) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return -1;
		}

		if (error)
			*error = SocketError::NoError;

		return code;
	}

	SocketState SocketImpl::Listen(SocketHandle handle, const IpAddress& address, unsigned int queueSize, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");
		NazaraAssert(address.IsValid(), "Invalid address");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		int bufferLength = IpAddressImpl::ToSockAddr(address, nameBuffer.data());

		if (bind(handle, reinterpret_cast<const sockaddr*>(&nameBuffer), bufferLength) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return SocketState::NotConnected;
		}

		if (listen(handle, queueSize) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return SocketState::NotConnected;
		}

		if (error)
			*error = SocketError::NoError;

		return SocketState::Bound;
	}

	std::size_t SocketImpl::QueryAvailableBytes(SocketHandle handle, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		int availableBytes;
		if (ioctl(handle, FIONREAD, &availableBytes) != 0)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return 0;
		}

		if (error)
			*error = SocketError::NoError;

		return SafeCast<std::size_t>(availableBytes);
	}

	bool SocketImpl::QueryBroadcasting(SocketHandle handle, SocketError* error)
	{
		int code;
		socklen_t codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_BROADCAST, &code, &codeLength) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false;
		}

		if (error)
			*error = SocketError::NoError;

		return code;
	}

	bool SocketImpl::QueryKeepAlive(SocketHandle handle, SocketError* error)
	{
		int code;
		socklen_t codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_KEEPALIVE, &code, &codeLength) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false;
		}

		if (error)
			*error = SocketError::NoError;

		return code;
	}

	std::size_t SocketImpl::QueryMaxDatagramSize(SocketHandle handle, SocketError* error)
	{
		// There's no SO_MAX_MSG_SIZE on POSIX
		// We could use IP_MTU but it requires a connected socket
		return std::min<std::size_t>(QuerySendBufferSize(handle, error), 65507); //< Max IPv4 value (IPv6 is 65527)
	}

	bool SocketImpl::QueryNoDelay(SocketHandle handle, SocketError* error)
	{
		int code;
		socklen_t codeLength = sizeof(code);

		if (getsockopt(handle, IPPROTO_TCP, TCP_NODELAY, &code, &codeLength) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false;
		}

		if (error)
			*error = SocketError::NoError;

		return code;
	}

	std::size_t SocketImpl::QueryReceiveBufferSize(SocketHandle handle, SocketError* error)
	{
		int code;
		socklen_t codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&code), &codeLength) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

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
		nameBuffer.fill(0);

		socklen_t bufferLength = sizeof(nameBuffer.size());

		if (getpeername(handle, reinterpret_cast<sockaddr*>(nameBuffer.data()), &bufferLength) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return IpAddress();
		}

		if (error)
			*error = SocketError::NoError;

		return IpAddressImpl::FromSockAddr(reinterpret_cast<sockaddr*>(nameBuffer.data()));
	}

	std::size_t SocketImpl::QuerySendBufferSize(SocketHandle handle, SocketError* error)
	{
		int code;
		socklen_t codeLength = sizeof(code);

		if (getsockopt(handle, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&code), &codeLength) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return 0;
		}

		if (error)
			*error = SocketError::NoError;

		return code;
	}

	IpAddress SocketImpl::QuerySocketAddress(SocketHandle handle, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		IpAddressImpl::SockAddrBuffer nameBuffer;
		nameBuffer.fill(0);

		socklen_t bufferLength = sizeof(sockaddr_in);

		if (getsockname(handle, reinterpret_cast<sockaddr*>(nameBuffer.data()), &bufferLength) == -1)
		{
			if (error)
			{
				int errorCode = errno;
				if (errorCode == EINVAL)
					*error = SocketError::NoError;
				else
					*error = TranslateErrorToSocketError(errorCode);
			}

			return IpAddress();
		}

		if (error)
			*error = SocketError::NoError;

		return IpAddressImpl::FromSockAddr(reinterpret_cast<sockaddr*>(nameBuffer.data()));
	}

	unsigned int SocketImpl::Poll(PollSocket* fdarray, std::size_t nfds, int timeout, SocketError* error)
	{
		NazaraAssert(fdarray && nfds > 0, "Invalid fdarray");

		static_assert(sizeof(PollSocket) == sizeof(pollfd), "PollSocket size must match WSAPOLLFD size");

		int result = poll(reinterpret_cast<pollfd*>(fdarray), static_cast<nfds_t>(nfds), timeout);
		if (result < 0)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return 0;
		}

		if (error)
			*error = SocketError::NoError;

		return static_cast<unsigned int>(result);
	}

	SocketState SocketImpl::PollConnection(SocketHandle handle, const IpAddress& /*address*/, UInt64 msTimeout, SocketError* error)
	{
		// Wait until socket is available for writing or an error occurs (ie when connection succeeds or fails)
		pollfd descriptor;
		descriptor.events = POLLOUT;
		descriptor.fd = handle;
		descriptor.revents = 0;

		int ret = ::poll(&descriptor, 1, (msTimeout != std::numeric_limits<UInt64>::max()) ? int(msTimeout) : -1);
		if (ret == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

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
			else if (descriptor.revents & POLLOUT)
				return SocketState::Connected;
			else
			{
				NazaraWarningFmt("socket {0} was returned by poll without POLLOUT nor error events (events: {1:#x})", handle, descriptor.revents);
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

		int byteRead = recv(handle, reinterpret_cast<char*>(buffer), length, 0);
		if (byteRead == -1)
		{
			int errorCode = errno;
			if (errorCode == EAGAIN)
				errorCode = EWOULDBLOCK;

			switch (errorCode)
			{
				case EWOULDBLOCK:
				{
					// If we have no data and are not blocking, return true with 0 byte read
					byteRead = 0;
					break;
				}

				default:
				{
					if (error)
						*error = TranslateErrorToSocketError(errorCode);

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
		nameBuffer.fill(0);

		socklen_t bufferLength = static_cast<socklen_t>(nameBuffer.size());

		IpAddress senderIp;

		int byteRead = recvfrom(handle, buffer, length, 0, reinterpret_cast<sockaddr*>(&nameBuffer), &bufferLength);
		if (byteRead == -1)
		{
			int errorCode = errno;
			if (errorCode == EAGAIN)
				errorCode = EWOULDBLOCK;

			switch (errorCode)
			{
				case EWOULDBLOCK:
				{
					// If we have no data and are not blocking, return true with 0 byte read
					byteRead = 0;
					senderIp = IpAddress::Invalid;
					break;
				}

				default:
				{
					if (error)
						*error = TranslateErrorToSocketError(errorCode);

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
			*from = IpAddressImpl::FromSockAddr(reinterpret_cast<const sockaddr*>(&nameBuffer));

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

		StackArray<iovec> sysBuffers = NazaraStackArray(iovec, bufferCount);
		for (std::size_t i = 0; i < bufferCount; ++i)
		{
			sysBuffers[i].iov_base = buffers[i].data;
			sysBuffers[i].iov_len = buffers[i].dataLength;
		}

		struct msghdr msgHdr;
		std::memset(&msgHdr, 0, sizeof(msgHdr));

		msgHdr.msg_iov = sysBuffers.data();
		msgHdr.msg_iovlen = static_cast<int>(bufferCount);

		IpAddressImpl::SockAddrBuffer nameBuffer;
		nameBuffer.fill(0);

		if (from)
		{
			msgHdr.msg_name = nameBuffer.data();
			msgHdr.msg_namelen = static_cast<socklen_t>(nameBuffer.size());
		}

		IpAddress senderIp;
#if defined(MSG_NOSIGNAL)
		int byteRead = recvmsg(handle, &msgHdr, MSG_NOSIGNAL);
#else
		int byteRead = recvmsg(handle, &msgHdr, 0);
#endif
		if (byteRead == -1)
		{
			int errorCode = errno;
			if (errorCode == EAGAIN)
				errorCode = EWOULDBLOCK;

			switch (errorCode)
			{
				case EWOULDBLOCK:
				{
					// If we have no data and are not blocking, return true with 0 byte read
					byteRead = 0;
					senderIp = IpAddress::Invalid;
					break;
				}

				default:
				{
					if (error)
						*error = TranslateErrorToSocketError(errorCode);

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
			senderIp = IpAddressImpl::FromSockAddr(reinterpret_cast<const sockaddr*>(nameBuffer.data()));

#ifdef HAS_MSGHDR_FLAGS
		if (msgHdr.msg_flags & MSG_TRUNC)
		{
			if (error)
				*error = SocketError::DatagramSize;

			return false;
		}
#endif

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
		if (byteSent == -1)
		{
			int errorCode = errno;
			if (errorCode == EAGAIN)
				errorCode = EWOULDBLOCK;

			switch (errorCode)
			{
				case EWOULDBLOCK:
					byteSent = 0;
					break;

				default:
				{
					if (error)
						*error = TranslateErrorToSocketError(errorCode);

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

		StackArray<iovec> sysBuffers = NazaraStackArray(iovec, bufferCount);
		for (std::size_t i = 0; i < bufferCount; ++i)
		{
			sysBuffers[i].iov_base = buffers[i].data;
			sysBuffers[i].iov_len = buffers[i].dataLength;
		}

		struct msghdr msgHdr;
		std::memset(&msgHdr, 0, sizeof(msgHdr));

		IpAddressImpl::SockAddrBuffer nameBuffer;
		msgHdr.msg_namelen = IpAddressImpl::ToSockAddr(to, nameBuffer.data());
		msgHdr.msg_name = nameBuffer.data();
		msgHdr.msg_iov = sysBuffers.data();
		msgHdr.msg_iovlen = static_cast<int>(bufferCount);

#if defined(MSG_NOSIGNAL)
		int byteSent = sendmsg(handle, &msgHdr, MSG_NOSIGNAL);
#else
		int byteSent = sendmsg(handle, &msgHdr, 0);
#endif

		if (byteSent == -1)
		{
			int errorCode = errno;
			if (errorCode == EAGAIN)
				errorCode = EWOULDBLOCK;

			switch (errorCode)
			{
				case EWOULDBLOCK:
					byteSent = 0;
					break;

				default:
				{
					if (error)
						*error = TranslateErrorToSocketError(errorCode);

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
		if (byteSent == -1)
		{
			int errorCode = errno;
			if (errorCode == EAGAIN)
				errorCode = EWOULDBLOCK;

			switch (errorCode)
			{
				case EWOULDBLOCK:
					byteSent = 0;
					break;

				default:
				{
					if (error)
						*error = TranslateErrorToSocketError(errorCode);

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
		if (ioctl(handle, FIONBIO, &block) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetBroadcasting(SocketHandle handle, bool broadcasting, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		int option = broadcasting;
		if (setsockopt(handle, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&option), sizeof(option)) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetIPv6Only(SocketHandle handle, bool ipv6Only, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		int option = ipv6Only;
		if (setsockopt(handle, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*>(&option), sizeof(option)) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetKeepAlive(SocketHandle handle, bool enabled, UInt64 msTime, UInt64 msInterval, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		int keepAlive = enabled ? 1 : 0;
		int keepIdle = msTime / 1000; // Linux works with seconds.
		int keepInterval = msInterval / 1000; // Linux works with seconds.

		if (setsockopt(handle, SOL_SOCKET, SO_KEEPALIVE, &keepAlive , sizeof(keepAlive)) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false; //< Error
		}

		if (setsockopt(handle, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle)) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false; //< Error
		}

		if (setsockopt(handle, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval)) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetNoDelay(SocketHandle handle, bool nodelay, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		int option = nodelay ? 1 : 0;
		if (setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, &option, sizeof(option)) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

#if not defined(MSG_NOSIGNAL) // -> https://github.com/intel/parameter-framework/pull/133/files
        //There is no MSG_NOSIGNAL on macos
        const int set = 1;
        if (setsockopt(handle, SOL_SOCKET, SO_NOSIGPIPE, &set, sizeof(set)) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false; //< Error
		}
#endif

		return true;
	}

	bool SocketImpl::SetReceiveBufferSize(SocketHandle handle, std::size_t size, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		int option = static_cast<int>(size);
		if (setsockopt(handle, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&option), sizeof(option)) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	bool SocketImpl::SetSendBufferSize(SocketHandle handle, std::size_t size, SocketError* error)
	{
		NazaraAssert(handle != InvalidHandle, "Invalid handle");

		int option = static_cast<int>(size);
		if (setsockopt(handle, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const char*>(&option), sizeof(option)) == -1)
		{
			if (error)
				*error = TranslateErrorToSocketError(errno);

			return false; //< Error
		}

		if (error)
			*error = SocketError::NoError;

		return true;
	}

	SocketError SocketImpl::TranslateErrorToSocketError(int error)
	{
		switch (error)
		{
			case 0:
				return SocketError::NoError;

			// Engine error
			case EACCES:
			case EBADF:
			case EINVAL:
			case EFAULT:
			case ENOTSOCK:
			case EPROTOTYPE:
			// Those are not errors and should have been handled
			case EALREADY:
			case EISCONN:
			case EWOULDBLOCK:
				NazaraWarningFmt("internal error occurred: {0} ({1})", Error::GetLastSystemError(error), error);
				return SocketError::Internal;

			case EADDRNOTAVAIL:
			case EADDRINUSE:
				return SocketError::AddressNotAvailable;

			case EAFNOSUPPORT:
			case EPFNOSUPPORT:
			case EOPNOTSUPP:
			case EPROTONOSUPPORT:
			case ESOCKTNOSUPPORT:
				return SocketError::NotSupported;

			case ECONNREFUSED:
				return SocketError::ConnectionRefused;

			case EINTR:
				return SocketError::Interrupted;

			case EMSGSIZE:
				return SocketError::DatagramSize;

			case EMFILE:
			case ENOBUFS:
			case ENOMEM:
				return SocketError::ResourceError;

			case ENOTCONN:
			case ESHUTDOWN:
				return SocketError::ConnectionClosed;

			case EHOSTUNREACH:
				return SocketError::UnreachableHost;

			case ENETDOWN:
			case ENETUNREACH:
				return SocketError::NetworkError;

			case ENODATA:
				return SocketError::NotInitialized;

			case ETIMEDOUT:
				return SocketError::TimedOut;
		}

		NazaraWarningFmt("unhandled POSIX error: {0} ({1})", Error::GetLastSystemError(error), error);
		return SocketError::Unknown;
	}

	int SocketImpl::TranslateNetProtocolToAF(NetProtocol protocol)
	{
		NazaraAssert(protocol <= NetProtocol::Max, "Protocol has value out of enum");

		constexpr EnumArray<NetProtocol, int> addressFamily {
			AF_UNSPEC, //< NetProtocol::Any
			AF_INET,   //< NetProtocol::IPv4
			AF_INET6,  //< NetProtocol::IPv6
			-1         //< NetProtocol::Unknown
		};

		return addressFamily[protocol];
	}

	int SocketImpl::TranslateSocketTypeToSock(SocketType type)
	{
		NazaraAssert(type <= SocketType::Max, "Socket type has value out of enum");

		constexpr EnumArray<SocketType, int> socketType {
			SOCK_RAW,     //< SocketType::Raw
			SOCK_STREAM,  //< SocketType::TCP
			SOCK_DGRAM,   //< SocketType::UDP
			-1            //< SocketType::Unknown
		};

		return socketType[type];
	}

	void SocketImpl::Uninitialize()
	{
	}

	SocketHandle SocketImpl::InvalidHandle = -1;
	SocketImpl::socketID SocketImpl::s_socket;
}

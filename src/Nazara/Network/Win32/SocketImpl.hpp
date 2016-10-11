// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOCKETIMPL_HPP
#define NAZARA_SOCKETIMPL_HPP

#include <Nazara/Network/SocketHandle.hpp>
#include <Nazara/Network/Enums.hpp>
#include <Nazara/Network/IpAddress.hpp>
#include <winsock2.h>

#define NAZARA_NETWORK_POLL_SUPPORT NAZARA_CORE_WINDOWS_NT6

namespace Nz
{
	struct PollSocket
	{
		SocketHandle fd;
		short events;
		short revents;
	};

	class SocketImpl
	{
		public:
			SocketImpl() = delete;
			~SocketImpl() = delete;

			static SocketHandle Accept(SocketHandle handle, IpAddress* address, SocketError* error);

			static SocketState Bind(SocketHandle handle, const IpAddress& address, SocketError* error);

			static SocketHandle Create(NetProtocol protocol, SocketType type, SocketError* error);
			
			static void ClearErrorCode(SocketHandle handle);
			static void Close(SocketHandle handle);

			static SocketState Connect(SocketHandle handle, const IpAddress& address, SocketError* error);
			static SocketState Connect(SocketHandle handle, const IpAddress& address, UInt64 msTimeout, SocketError* error);

			static bool Initialize();

			static SocketError GetLastError(SocketHandle handle, SocketError* error = nullptr);
			static int GetLastErrorCode();
			static int GetLastErrorCode(SocketHandle handle, SocketError* error = nullptr);

			static SocketState Listen(SocketHandle handle, const IpAddress& address, unsigned queueSize, SocketError* error);

			static std::size_t QueryAvailableBytes(SocketHandle handle, SocketError* error = nullptr);
			static bool QueryBroadcasting(SocketHandle handle, SocketError* error = nullptr);
			static bool QueryKeepAlive(SocketHandle handle, SocketError* error = nullptr);
			static std::size_t QueryMaxDatagramSize(SocketHandle handle, SocketError* error = nullptr);
			static bool QueryNoDelay(SocketHandle handle, SocketError* error = nullptr);
			static IpAddress QueryPeerAddress(SocketHandle handle, SocketError* error = nullptr);
			static IpAddress QuerySocketAddress(SocketHandle handle, SocketError* error = nullptr);

			static int Poll(PollSocket* fdarray, std::size_t nfds, int timeout, SocketError* error);

			static bool Receive(SocketHandle handle, void* buffer, int length, int* read, SocketError* error);
			static bool ReceiveFrom(SocketHandle handle, void* buffer, int length, IpAddress* from, int* read, SocketError* error);

			static bool Send(SocketHandle handle, const void* buffer, int length, int* sent, SocketError* error);
			static bool SendTo(SocketHandle handle, const void* buffer, int length, const IpAddress& to, int* sent, SocketError* error);

			static bool SetBlocking(SocketHandle handle, bool blocking, SocketError* error = nullptr);
			static bool SetBroadcasting(SocketHandle handle, bool broadcasting, SocketError* error = nullptr);
			static bool SetKeepAlive(SocketHandle handle, bool enabled, UInt64 msTime, UInt64 msInterval, SocketError* error = nullptr);
			static bool SetNoDelay(SocketHandle handle, bool nodelay, SocketError* error = nullptr);

			static SocketError TranslateWSAErrorToSocketError(int error);
			static int TranslateNetProtocolToAF(NetProtocol protocol);
			static int TranslateSocketTypeToSock(SocketType type);

			static void Uninitialize();

			static SocketHandle InvalidHandle;

		private:
			static WSADATA s_WSA;
	};
}

#endif // NAZARA_SOCKETIMPL_HPP
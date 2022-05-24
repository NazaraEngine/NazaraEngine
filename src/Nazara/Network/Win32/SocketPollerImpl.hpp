// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_NETWORK_WIN32_SOCKETPOLLERIMPL_HPP
#define NAZARA_NETWORK_WIN32_SOCKETPOLLERIMPL_HPP

#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Network/SocketHandle.hpp>
#include <Nazara/Network/Win32/SocketImpl.hpp>
#include <WinSock2.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Nz
{
	class SocketPollerImpl
	{
		public:
			SocketPollerImpl();
			~SocketPollerImpl() = default;

			void Clear();

			bool IsReadyToRead(SocketHandle socket) const;
			bool IsReadyToWrite(SocketHandle socket) const;
			bool IsRegistered(SocketHandle socket) const;

			bool RegisterSocket(SocketHandle socket, SocketPollEventFlags eventFlags);
			void UnregisterSocket(SocketHandle socket);

			unsigned int Wait(int msTimeout, SocketError* error);

		private:
			#if NAZARA_NETWORK_POLL_SUPPORT
			std::unordered_set<SocketHandle> m_readyToReadSockets;
			std::unordered_set<SocketHandle> m_readyToWriteSockets;
			std::unordered_map<SocketHandle, std::size_t> m_allSockets;
			std::vector<PollSocket> m_sockets;
			#else
			fd_set m_readSockets;
			fd_set m_readyToReadSockets;
			fd_set m_readyToWriteSockets;
			fd_set m_writeSockets;
			#endif
	};
}

#endif // NAZARA_NETWORK_WIN32_SOCKETPOLLERIMPL_HPP

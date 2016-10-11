// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOCKETPOLLERIMPL_HPP
#define NAZARA_SOCKETPOLLERIMPL_HPP

#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Network/SocketHandle.hpp>
#include <Nazara/Network/Win32/SocketImpl.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <winsock2.h>

namespace Nz
{
	class SocketPollerImpl
	{
		public:
			SocketPollerImpl();
			~SocketPollerImpl() = default;

			void Clear();

			bool IsReady(SocketHandle socket) const;
			bool IsRegistered(SocketHandle socket) const;

			bool RegisterSocket(SocketHandle socket);
			void UnregisterSocket(SocketHandle socket);

			int Wait(UInt64 msTimeout, SocketError* error);

		private:
			#if NAZARA_NETWORK_POLL_SUPPORT
			std::unordered_set<SocketHandle> m_activeSockets;
			std::unordered_map<SocketHandle, std::size_t> m_allSockets;
			std::vector<PollSocket> m_sockets;
			#else
			fd_set m_sockets;
			fd_set m_activeSockets;
			#endif
	};
}

#endif // NAZARA_SOCKETPOLLERIMPL_HPP
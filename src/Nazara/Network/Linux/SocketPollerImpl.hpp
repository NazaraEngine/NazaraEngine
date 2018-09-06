// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOCKETPOLLERIMPL_HPP
#define NAZARA_SOCKETPOLLERIMPL_HPP

#include <Nazara/Network/IpAddress.hpp>
#include <Nazara/Network/SocketHandle.hpp>
#include <unordered_set>
#include <vector>
#include <sys/epoll.h>

namespace Nz
{
	class SocketPollerImpl
	{
		public:
			SocketPollerImpl();
			~SocketPollerImpl();

			void Clear();

			bool IsReadyToRead(SocketHandle socket) const;
			bool IsReadyToWrite(SocketHandle socket) const;
			bool IsRegistered(SocketHandle socket) const;

			bool RegisterSocket(SocketHandle socket, SocketPollEventFlags eventFlags);
			void UnregisterSocket(SocketHandle socket);

			unsigned int Wait(int msTimeout, SocketError* error);

		private:
			std::unordered_set<SocketHandle> m_readyToReadSockets;
			std::unordered_set<SocketHandle> m_readyToWriteSockets;
			std::unordered_set<SocketHandle> m_sockets;
			std::vector<epoll_event> m_events;
			int m_handle;
	};
}

#endif // NAZARA_SOCKETPOLLERIMPL_HPP

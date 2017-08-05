// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOCKETPOLLER_HPP
#define NAZARA_SOCKETPOLLER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Network/AbstractSocket.hpp>
#include <Nazara/Network/IpAddress.hpp>

namespace Nz
{
	class SocketPollerImpl;

	class NAZARA_NETWORK_API SocketPoller
	{
		public:
			SocketPoller();
			inline SocketPoller(SocketPoller&& socketPoller);
			~SocketPoller();

			void Clear();

			bool IsReadyToRead(const AbstractSocket& socket) const;
			bool IsReadyToWrite(const AbstractSocket& socket) const;
			bool IsRegistered(const AbstractSocket& socket) const;

			bool RegisterSocket(AbstractSocket& socket, SocketPollEventFlags eventFlags);
			void UnregisterSocket(AbstractSocket& socket);

			bool Wait(int msTimeout);

			inline SocketPoller& operator=(SocketPoller&& socketPoller);

		private:
			SocketPollerImpl* m_impl;
	};
}

#include <Nazara/Network/SocketPoller.inl>

#endif // NAZARA_SOCKETPOLLER_HPP

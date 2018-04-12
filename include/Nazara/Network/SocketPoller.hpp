// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SOCKETPOLLER_HPP
#define NAZARA_SOCKETPOLLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Network/AbstractSocket.hpp>

namespace Nz
{
	class SocketPollerImpl;

	class NAZARA_NETWORK_API SocketPoller
	{
		public:
			SocketPoller();
			SocketPoller(SocketPoller&&) noexcept = default;
			~SocketPoller();

			void Clear();

			bool IsReadyToRead(const AbstractSocket& socket) const;
			bool IsReadyToWrite(const AbstractSocket& socket) const;
			bool IsRegistered(const AbstractSocket& socket) const;

			bool RegisterSocket(AbstractSocket& socket, SocketPollEventFlags eventFlags);
			void UnregisterSocket(AbstractSocket& socket);

			bool Wait(int msTimeout);

			SocketPoller& operator=(SocketPoller&&) noexcept = default;

		private:
			MovablePtr<SocketPollerImpl> m_impl;
	};
}

#include <Nazara/Network/SocketPoller.inl>

#endif // NAZARA_SOCKETPOLLER_HPP

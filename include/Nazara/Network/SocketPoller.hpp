// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_NETWORK_SOCKETPOLLER_HPP
#define NAZARA_NETWORK_SOCKETPOLLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Network/AbstractSocket.hpp>
#include <NazaraUtils/MovablePtr.hpp>

namespace Nz
{
	class SocketPollerImpl;

	class NAZARA_NETWORK_API SocketPoller
	{
		public:
			SocketPoller();
			SocketPoller(const SocketPoller&) = delete;
			SocketPoller(SocketPoller&&) noexcept = default;
			~SocketPoller();

			void Clear();

			bool IsReadyToRead(const AbstractSocket& socket) const;
			bool IsReadyToWrite(const AbstractSocket& socket) const;
			bool IsRegistered(const AbstractSocket& socket) const;

			bool RegisterSocket(AbstractSocket& socket, SocketPollEventFlags eventFlags);
			void UnregisterSocket(AbstractSocket& socket);

			unsigned int Wait(int msTimeout, SocketError* error = nullptr);

			SocketPoller& operator=(const SocketPoller&) = delete;
			SocketPoller& operator=(SocketPoller&&) noexcept = default;

		private:
			MovablePtr<SocketPollerImpl> m_impl;
	};
}

#include <Nazara/Network/SocketPoller.inl>

#endif // NAZARA_NETWORK_SOCKETPOLLER_HPP

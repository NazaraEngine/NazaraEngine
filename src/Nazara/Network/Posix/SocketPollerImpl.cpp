// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Posix/SocketPollerImpl.hpp>
#include <poll.h>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	void SocketPollerImpl::Clear()
	{
		m_activeSockets.clear();
		m_allSockets.clear();
		m_sockets.clear();
	}

	bool SocketPollerImpl::IsReady(SocketHandle socket) const
	{
		return m_activeSockets.count(socket) != 0;
	}

	bool SocketPollerImpl::IsRegistered(SocketHandle socket) const
	{
		return m_allSockets.count(socket) != 0;
	}

	bool SocketPollerImpl::RegisterSocket(SocketHandle socket)
	{
		NazaraAssert(!IsRegistered(socket), "Socket is already registered");

		PollSocket entry = {
			socket,
			POLLRDNORM,
			0
		};

		m_allSockets[socket] = m_sockets.size();
		m_sockets.emplace_back(entry);

		return true;
	}

	void SocketPollerImpl::UnregisterSocket(SocketHandle socket)
	{
		NazaraAssert(IsRegistered(socket), "Socket is not registered");

		if (m_sockets.size() > 1U)
		{
			// Instead of using vector::erase, let's move the last element to the now unoccupied position
			std::size_t entry = m_allSockets[socket];

			// Get the last element and update it's position
			const PollSocket& lastElement = m_sockets.back();
			m_allSockets[lastElement.fd] = entry;

			// Now move it properly (lastElement is invalid after the following line) and pop it
			m_sockets[entry] = std::move(m_sockets.back());
		}

		m_sockets.pop_back();
		m_activeSockets.erase(socket);
		m_allSockets.erase(socket);
	}

	int SocketPollerImpl::Wait(UInt64 msTimeout, SocketError* error)
	{
		int activeSockets;

		// Reset status of sockets
		for (PollSocket& entry : m_sockets)
			entry.revents = 0;

		activeSockets = SocketImpl::Poll(m_sockets.data(), m_sockets.size(), static_cast<int>(msTimeout), error);

		m_activeSockets.clear();
		if (activeSockets > 0U)
		{
			int socketRemaining = activeSockets;
			for (PollSocket& entry : m_sockets)
			{
				if (entry.revents & POLLRDNORM)
				{
					m_activeSockets.insert(entry.fd);
					if (--socketRemaining == 0)
						break;
				}
			}
		}

		return activeSockets;
	}
}

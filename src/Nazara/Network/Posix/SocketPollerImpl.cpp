// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Network/Posix/SocketPollerImpl.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <poll.h>

namespace Nz
{
	void SocketPollerImpl::Clear()
	{
		m_readyToReadSockets.clear();
		m_readyToWriteSockets.clear();
		m_allSockets.clear();
		m_sockets.clear();
	}

	bool SocketPollerImpl::IsReadyToRead(SocketHandle socket) const
	{
		return m_readyToReadSockets.count(socket) != 0;
	}

	bool SocketPollerImpl::IsReadyToWrite(SocketHandle socket) const
	{
		return m_readyToWriteSockets.count(socket) != 0;
	}

	bool SocketPollerImpl::IsRegistered(SocketHandle socket) const
	{
		return m_allSockets.count(socket) != 0;
	}

	bool SocketPollerImpl::RegisterSocket(SocketHandle socket, SocketPollEventFlags eventFlags)
	{
		NazaraAssert(!IsRegistered(socket), "Socket is already registered");

		PollSocket entry = {
			socket,
			0,
			0
		};

		if (eventFlags & SocketPollEvent::Read)
			entry.events |= POLLRDNORM;

		if (eventFlags & SocketPollEvent::Write)
			entry.events |= POLLWRNORM;

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

		m_allSockets.erase(socket);
		m_readyToReadSockets.erase(socket);
		m_readyToWriteSockets.erase(socket);
	}

	unsigned int SocketPollerImpl::Wait(int msTimeout, SocketError* error)
	{
		unsigned int activeSockets;

		// Reset status of sockets
		activeSockets = SocketImpl::Poll(m_sockets.data(), m_sockets.size(), static_cast<int>(msTimeout), error);

		m_readyToReadSockets.clear();
		m_readyToWriteSockets.clear();
		if (activeSockets > 0U)
		{
			unsigned int socketRemaining = activeSockets;
			for (PollSocket& entry : m_sockets)
			{
				if (!entry.revents)
					continue;

				if (entry.revents & (POLLRDNORM | POLLWRNORM | POLLHUP | POLLERR))
				{
					if (entry.revents & (POLLRDNORM | POLLHUP | POLLERR))
						m_readyToReadSockets.insert(entry.fd);

					if (entry.revents & (POLLWRNORM | POLLERR))
						m_readyToWriteSockets.insert(entry.fd);
				}
				else
				{
					NazaraWarning("Socket {0} was returned by poll without POLLRDNORM nor POLLWRNORM events (events: {1:#x})", entry.fd, entry.revents);
					activeSockets--;
				}

				entry.revents = 0;

				if (--socketRemaining == 0)
					break;
			}
		}

		return activeSockets;
	}
}

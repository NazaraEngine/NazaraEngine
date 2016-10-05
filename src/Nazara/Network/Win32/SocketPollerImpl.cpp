// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Win32/SocketPollerImpl.hpp>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	SocketPollerImpl::SocketPollerImpl()
	{
		#if !NAZARA_NETWORK_POLL_SUPPORT
		FD_ZERO(&m_activeSockets);
		FD_ZERO(&m_sockets);
		#endif
	}

	void SocketPollerImpl::Clear()
	{
		#if NAZARA_NETWORK_POLL_SUPPORT
		m_activeSockets.clear();
		m_allSockets.clear();
		m_sockets.clear();
		#else
		FD_ZERO(&m_activeSockets);
		FD_ZERO(&m_sockets);
		#endif
	}

	bool SocketPollerImpl::IsReady(SocketHandle socket) const
	{
		#if NAZARA_NETWORK_POLL_SUPPORT
		return m_activeSockets.count(socket) != 0;
		#else
		return FD_ISSET(socket, &m_activeSockets) != 0;
		#endif
	}

	bool SocketPollerImpl::IsRegistered(SocketHandle socket) const
	{
		#if NAZARA_NETWORK_POLL_SUPPORT
		return m_allSockets.count(socket) != 0;
		#else
		return FD_ISSET(socket, &m_sockets) != 0;
		#endif
	}

	bool SocketPollerImpl::RegisterSocket(SocketHandle socket)
	{
		NazaraAssert(!IsRegistered(socket), "Socket is already registered");

		#if NAZARA_NETWORK_POLL_SUPPORT
		PollSocket entry = {
			socket,
			POLLRDNORM,
			0
		};

		m_allSockets[socket] = m_sockets.size();
		m_sockets.emplace_back(entry);
		#else
		if (m_sockets.fd_count > FD_SETSIZE)
		{
			NazaraError("Socket count exceeding FD_SETSIZE (" + String::Number(FD_SETSIZE) + ")");
			return false;
		}

		FD_SET(socket, &m_sockets);
		#endif

		return true;
	}

	void SocketPollerImpl::UnregisterSocket(SocketHandle socket)
	{
		NazaraAssert(IsRegistered(socket), "Socket is not registered");

		#if NAZARA_NETWORK_POLL_SUPPORT
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
		#else
		FD_CLR(socket, &m_activeSockets);
		FD_CLR(socket, &m_sockets);
		#endif
	}

	int SocketPollerImpl::Wait(UInt64 msTimeout, SocketError* error)
	{
		int activeSockets;

		#if NAZARA_NETWORK_POLL_SUPPORT
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
		#else

		m_activeSockets = m_sockets;

		timeval tv;
		tv.tv_sec = static_cast<long>(msTimeout / 1000ULL);
		tv.tv_usec = static_cast<long>((msTimeout % 1000ULL) * 1000ULL);

		activeSockets = ::select(0xDEADBEEF, &m_activeSockets, nullptr, nullptr, (msTimeout > 0) ? &tv : nullptr); //< The first argument is ignored on Windows
		if (activeSockets == SOCKET_ERROR)
		{
			if (error)
				*error = SocketImpl::TranslateWSAErrorToSocketError(WSAGetLastError());

			return 0;
		}

		if (error)
			*error = SocketError_NoError;
		#endif

		return activeSockets;
	}
}

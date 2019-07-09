// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Win32/SocketPollerImpl.hpp>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	SocketPollerImpl::SocketPollerImpl()
	{
		#if !NAZARA_NETWORK_POLL_SUPPORT
		FD_ZERO(&m_readSockets);
		FD_ZERO(&m_readyToReadSockets);
		FD_ZERO(&m_readyToWriteSockets);
		FD_ZERO(&m_writeSockets);
		#endif
	}

	void SocketPollerImpl::Clear()
	{
		#if NAZARA_NETWORK_POLL_SUPPORT
		m_allSockets.clear();
		m_readyToReadSockets.clear();
		m_readyToWriteSockets.clear();
		m_sockets.clear();
		#else
		FD_ZERO(&m_readSockets);
		FD_ZERO(&m_readyToReadSockets);
		FD_ZERO(&m_readyToWriteSockets);
		FD_ZERO(&m_writeSockets);
		#endif
	}

	bool SocketPollerImpl::IsReadyToRead(SocketHandle socket) const
	{
		#if NAZARA_NETWORK_POLL_SUPPORT
		return m_readyToReadSockets.count(socket) != 0;
		#else
		return FD_ISSET(socket, const_cast<fd_set*>(&m_readyToReadSockets)) != 0; //< FD_ISSET is not const-correct
		#endif
	}

	bool SocketPollerImpl::IsReadyToWrite(SocketHandle socket) const
	{
		#if NAZARA_NETWORK_POLL_SUPPORT
		return m_readyToWriteSockets.count(socket) != 0;
		#else
		return FD_ISSET(socket, const_cast<fd_set*>(&m_readyToWriteSockets)) != 0; //< FD_ISSET is not const-correct
		#endif
	}

	bool SocketPollerImpl::IsRegistered(SocketHandle socket) const
	{
		#if NAZARA_NETWORK_POLL_SUPPORT
		return m_allSockets.count(socket) != 0;
		#else
		// FD_ISSET is not const-correct
		return FD_ISSET(socket, const_cast<fd_set*>(&m_readSockets)) != 0 || 
		       FD_ISSET(socket, const_cast<fd_set*>(&m_writeSockets)) != 0;
		#endif
	}

	bool SocketPollerImpl::RegisterSocket(SocketHandle socket, SocketPollEventFlags eventFlags)
	{
		NazaraAssert(!IsRegistered(socket), "Socket is already registered");

		#if NAZARA_NETWORK_POLL_SUPPORT
		PollSocket entry = {
			socket,
			0,
			0
		};

		if (eventFlags & SocketPollEvent_Read)
			entry.events |= POLLRDNORM;

		if (eventFlags & SocketPollEvent_Write)
			entry.events |= POLLWRNORM;

		m_allSockets[socket] = m_sockets.size();
		m_sockets.emplace_back(entry);
		#else
		for (std::size_t i = 0; i < 2; ++i)
		{
			if ((eventFlags & ((i == 0) ? SocketPollEvent_Read : SocketPollEvent_Write)) == 0)
				continue;

			fd_set& targetSet = (i == 0) ? m_readSockets : m_writeSockets;
			if (targetSet.fd_count > FD_SETSIZE)
			{
				NazaraError("Socket count exceeding hard-coded FD_SETSIZE (" + String::Number(FD_SETSIZE) + ")");
				return false;
			}

			FD_SET(socket, &targetSet);
		}
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

		m_allSockets.erase(socket);
		m_readyToReadSockets.erase(socket);
		m_readyToWriteSockets.erase(socket);
		#else
		FD_CLR(socket, &m_readSockets);
		FD_CLR(socket, &m_readyToReadSockets);
		FD_CLR(socket, &m_readyToWriteSockets);
		FD_CLR(socket, &m_writeSockets);
		#endif
	}

	unsigned int SocketPollerImpl::Wait(int msTimeout, SocketError* error)
	{
		unsigned int activeSockets;

		#if NAZARA_NETWORK_POLL_SUPPORT
		activeSockets = SocketImpl::Poll(m_sockets.data(), m_sockets.size(), static_cast<int>(msTimeout), error);

		m_readyToReadSockets.clear();
		m_readyToWriteSockets.clear();
		if (activeSockets > 0U)
		{
			int socketRemaining = activeSockets;
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
					NazaraWarning("Socket " + String::Number(entry.fd) + " was returned by WSAPoll without POLLRDNORM nor POLLWRNORM events (events: 0x" + String::Number(entry.revents, 16) + ')');
					activeSockets--;
				}

				entry.revents = 0;

				if (--socketRemaining == 0)
					break;
			}
		}
		#else
		fd_set* readSet = nullptr;
		fd_set* writeSet = nullptr;

		if (m_readSockets.fd_count > 0)
		{
			m_readyToReadSockets = m_readSockets;
			readSet = &m_readyToReadSockets;
		}

		if (m_writeSockets.fd_count > 0)
		{
			m_readyToWriteSockets = m_writeSockets;
			readSet = &m_readyToWriteSockets;
		}

		timeval tv;
		tv.tv_sec = static_cast<long>(msTimeout / 1000ULL);
		tv.tv_usec = static_cast<long>((msTimeout % 1000ULL) * 1000ULL);

		int selectValue = ::select(0xDEADBEEF, readSet, writeSet, nullptr, (msTimeout >= 0) ? &tv : nullptr); //< The first argument is ignored on Windows
		if (selectValue == SOCKET_ERROR)
		{
			if (error)
				*error = SocketImpl::TranslateWSAErrorToSocketError(WSAGetLastError());

			return 0;
		}

		assert(selectValue >= 0);
		activeSockets = static_cast<unsigned int>(selectValue);

		if (error)
			*error = SocketError_NoError;
		#endif

		return activeSockets;
	}
}

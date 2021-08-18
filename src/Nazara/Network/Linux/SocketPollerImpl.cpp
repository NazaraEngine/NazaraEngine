// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Linux/SocketPollerImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Network/Posix/SocketImpl.hpp>
#include <cstring>
#include <unistd.h>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	SocketPollerImpl::SocketPollerImpl()
	{
		m_handle = epoll_create1(0);
	}

	SocketPollerImpl::~SocketPollerImpl()
	{
		close(m_handle);
	}

	void SocketPollerImpl::Clear()
	{
		m_readyToReadSockets.clear();
		m_readyToWriteSockets.clear();
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
		return m_sockets.count(socket) != 0;
	}

	bool SocketPollerImpl::RegisterSocket(SocketHandle socket, SocketPollEventFlags eventFlags)
	{
		NazaraAssert(!IsRegistered(socket), "Socket is already registered");

		epoll_event entry;
		std::memset(&entry, 0, sizeof(epoll_event));

		entry.data.fd = socket;

		if (eventFlags & SocketPollEvent::Read)
			entry.events |= EPOLLIN;

		if (eventFlags & SocketPollEvent::Write)
			entry.events |= EPOLLOUT;

		if (epoll_ctl(m_handle, EPOLL_CTL_ADD, socket, &entry) != 0)
		{
			NazaraError("Failed to add socket to epoll structure (errno " + NumberToString(errno) + ": " + Error::GetLastSystemError() + ')');
			return false;
		}

		m_sockets.insert(socket);

		return true;
	}

	void SocketPollerImpl::UnregisterSocket(SocketHandle socket)
	{
		NazaraAssert(IsRegistered(socket), "Socket is not registered");

		m_readyToReadSockets.erase(socket);
		m_readyToWriteSockets.erase(socket);
		m_sockets.erase(socket);

		if (epoll_ctl(m_handle, EPOLL_CTL_DEL, socket, nullptr) != 0)
			NazaraWarning("An error occured while removing socket from epoll structure (errno " + NumberToString(errno) + ": " + Error::GetLastSystemError() + ')');
	}

	unsigned int SocketPollerImpl::Wait(int msTimeout, SocketError* error)
	{
		int activeSockets;

		// Reset status of sockets
		m_events.resize(m_sockets.size());
		std::memset(m_events.data(), 0, m_events.size() * sizeof(epoll_event));

		activeSockets = epoll_wait(m_handle, m_events.data(), static_cast<int>(m_events.size()), static_cast<int>(msTimeout));
		if (activeSockets == -1)
		{
			if (error)
				*error = SocketImpl::TranslateErrnoToSocketError(errno);

			return 0;
		}

		m_readyToReadSockets.clear();
		m_readyToWriteSockets.clear();
		if (activeSockets > 0)
		{
			int socketCount = activeSockets;
			for (int i = 0; i < socketCount; ++i)
			{
				if (m_events[i].events & (EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR))
				{
					if (m_events[i].events & (EPOLLIN | EPOLLHUP | EPOLLERR))
						m_readyToReadSockets.insert(m_events[i].data.fd);

					if (m_events[i].events & (EPOLLOUT | EPOLLERR))
						m_readyToWriteSockets.insert(m_events[i].data.fd);
				}
				else
				{
					NazaraWarning("Descriptor " + NumberToString(m_events[i].data.fd) + " was returned by epoll without EPOLLIN nor EPOLLOUT flags (events: 0x" + NumberToString(m_events[i].events, 16) + ')');
					activeSockets--;
				}
			}
		}

		if (error)
			*error = SocketError::NoError;

		return activeSockets;
	}
}

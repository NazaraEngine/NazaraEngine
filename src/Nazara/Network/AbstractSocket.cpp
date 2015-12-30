// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/AbstractSocket.hpp>
#include <Nazara/Core/Error.hpp>
#include <utility>
#include <Nazara/Network/Debug.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/SocketImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/SocketImpl.hpp>
#else
#error Missing implementation: Socket
#endif

namespace Nz
{
	AbstractSocket::AbstractSocket(SocketType type) :
	m_handle(SocketImpl::InvalidHandle),
	m_state(SocketState_NotConnected),
	m_type(type),
	m_isBlockingEnabled(true)
	{
	}

	AbstractSocket::AbstractSocket(AbstractSocket&& abstractSocket) :
	m_protocol(abstractSocket.m_protocol),
	m_lastError(abstractSocket.m_lastError),
	m_handle(abstractSocket.m_handle),
	m_state(abstractSocket.m_state),
	m_type(abstractSocket.m_type),
	m_isBlockingEnabled(abstractSocket.m_isBlockingEnabled)
	{
		abstractSocket.m_handle = SocketImpl::InvalidHandle;
	}

	AbstractSocket::~AbstractSocket()
	{
		Close();
	}

	void AbstractSocket::Close()
	{
		if (m_handle != SocketImpl::InvalidHandle)
		{
			OnClose();

			SocketImpl::Close(m_handle);
			m_handle = SocketImpl::InvalidHandle;
		}
	}

	void AbstractSocket::EnableBlocking(bool blocking)
	{
		if (m_isBlockingEnabled != blocking)
		{
			if (m_handle != SocketImpl::InvalidHandle)
				SocketImpl::SetBlocking(m_handle, blocking, &m_lastError);

			m_isBlockingEnabled = blocking;
		}
	}

	unsigned int AbstractSocket::QueryAvailableBytes() const
	{
		if (m_handle == SocketImpl::InvalidHandle)
			return 0;

		return SocketImpl::QueryAvailableBytes(m_handle);
	}

	void AbstractSocket::OnClose()
	{
		UpdateState(SocketState_NotConnected);
	}

	void AbstractSocket::OnOpened()
	{
		SocketError errorCode;
		if (!SocketImpl::SetBlocking(m_handle, m_isBlockingEnabled, &errorCode))
			NazaraWarning("Failed to set socket blocking mode (0x" + String::Number(errorCode, 16) + ')');
	}

	bool AbstractSocket::Open(NetProtocol protocol)
	{
		if (m_handle == SocketImpl::InvalidHandle || m_protocol != protocol)
		{
			SocketHandle handle = SocketImpl::Create(protocol, m_type, &m_lastError);
			if (handle == SocketImpl::InvalidHandle)
				return false;

			m_protocol = protocol;
			Open(handle);
		}

		return true;
	}

	void AbstractSocket::Open(SocketHandle handle)
	{
		NazaraAssert(handle != SocketImpl::InvalidHandle, "Invalid handle");

		Close();

		m_handle = handle;
		OnOpened();
	}
}

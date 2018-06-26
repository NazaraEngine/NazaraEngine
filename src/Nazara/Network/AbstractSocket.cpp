// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/AbstractSocket.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Network/Algorithm.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/SocketImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/SocketImpl.hpp>
#else
#error Missing implementation: Socket
#endif

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup network
	* \class Nz::AbstractSocket
	* \brief Network class that represents the base of socket
	*
	* \remark This class is abstract
	*/

	/*!
	* \brief Constructs a AbstractSocket object with a type
	*/

	AbstractSocket::AbstractSocket(SocketType type) :
	m_lastError(SocketError_NoError),
	m_handle(SocketImpl::InvalidHandle),
	m_state(SocketState_NotConnected),
	m_type(type),
	m_isBlockingEnabled(true)
	{
	}

	/*!
	* \brief Constructs a AbstractSocket object with another one by move semantic
	*
	* \param abstractSocket AbstractSocket to move into this
	*/

	AbstractSocket::AbstractSocket(AbstractSocket&& abstractSocket) noexcept :
	m_protocol(abstractSocket.m_protocol),
	m_lastError(abstractSocket.m_lastError),
	m_handle(abstractSocket.m_handle),
	m_state(abstractSocket.m_state),
	m_type(abstractSocket.m_type),
	m_isBlockingEnabled(abstractSocket.m_isBlockingEnabled)
	{
		abstractSocket.m_handle = SocketImpl::InvalidHandle;
	}

	/*!
	* \brief Destructs the object and calls Close
	*
	* \see Close
	*/

	AbstractSocket::~AbstractSocket()
	{
		Close();
	}

	/*!
	* \brief Closes the socket
	*/

	void AbstractSocket::Close()
	{
		if (m_handle != SocketImpl::InvalidHandle)
		{
			OnClose();

			SocketImpl::Close(m_handle);
			m_handle = SocketImpl::InvalidHandle;
		}
	}

	/*!
	* \brief Enables blocking
	*
	* \param blocking Should the read block
	*/

	void AbstractSocket::EnableBlocking(bool blocking)
	{
		if (m_isBlockingEnabled != blocking)
		{
			if (m_handle != SocketImpl::InvalidHandle)
				SocketImpl::SetBlocking(m_handle, blocking, &m_lastError);

			m_isBlockingEnabled = blocking;
		}
	}

	/*!
	* \brief Queries the available bytes
	* \return Number of bytes which can be read
	*/

	std::size_t AbstractSocket::QueryAvailableBytes() const
	{
		if (m_handle == SocketImpl::InvalidHandle)
			return 0;

		return SocketImpl::QueryAvailableBytes(m_handle);
	}

	/*!
	* \brief Queries the maximum socket receive buffer size
	* \return The size of the receive buffer in bytes.
	*/
	std::size_t AbstractSocket::QueryReceiveBufferSize() const
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Socket must be created first");

		return SocketImpl::QueryReceiveBufferSize(m_handle);
	}

	/*!
	* \brief Queries the maximum socket send buffer size
	* \return The size of the send buffer in bytes.
	*/
	std::size_t AbstractSocket::QuerySendBufferSize() const
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Socket must be created first");

		return SocketImpl::QuerySendBufferSize(m_handle);
	}

	/*!
	* \brief Sets the maximum receive buffer size
	*
	* \param size The new maximum receive buffer size in bytes
	*/
	void AbstractSocket::SetReceiveBufferSize(std::size_t size)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Socket must be created first");
		
		SocketImpl::SetReceiveBufferSize(m_handle, size);
	}

	/*!
	* \brief Sets the maximum send buffer size
	*
	* \param size The new maximum send buffer size in bytes
	*/
	void AbstractSocket::SetSendBufferSize(std::size_t size)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Socket must be created first");
		
		SocketImpl::SetSendBufferSize(m_handle, size);
	}

	/*!
	* \brief Operation to do when closing socket
	*/

	void AbstractSocket::OnClose()
	{
		UpdateState(SocketState_NotConnected);
	}

	/*!
	* \brief Operation to do when opening socket
	*
	* \remark Produces a NazaraWarning if blocking failed
	*/

	void AbstractSocket::OnOpened()
	{
		SocketError errorCode;
		if (!SocketImpl::SetBlocking(m_handle, m_isBlockingEnabled, &errorCode))
			NazaraWarning("Failed to set socket blocking mode (0x" + String::Number(errorCode, 16) + ')');
	}

	/*!
	* \brief Opens the socket according to a net protocol
	* \return true If successful
	*/

	bool AbstractSocket::Open(NetProtocol protocol)
	{
		if (m_handle == SocketImpl::InvalidHandle || m_protocol != protocol)
		{
			SocketHandle handle = SocketImpl::Create((protocol == NetProtocol_Any) ? NetProtocol_IPv6 : protocol, m_type, &m_lastError);
			if (handle == SocketImpl::InvalidHandle)
				return false;

			if (protocol == NetProtocol_Any)
			{
				if (!SocketImpl::SetIPv6Only(handle, false, &m_lastError))
				{
					SocketImpl::Close(handle);

					NazaraError("Failed to open a dual-stack socket: " + Nz::String(ErrorToString(m_lastError)));
					return false;
				}

				protocol = NetProtocol_IPv6;
			}

			m_protocol = protocol;
			Open(handle);
		}

		return true;
	}

	/*!
	* \brief Opens the socket according to a socket handle
	* \return true If successful
	*
	* \remark Produces a NazaraAssert if handle is invalid
	*/

	void AbstractSocket::Open(SocketHandle handle)
	{
		NazaraAssert(handle != SocketImpl::InvalidHandle, "Invalid handle");

		Close();

		m_handle = handle;
		OnOpened();
	}

	/*!
	* \brief Moves the AbstractSocket into this
	* \return A reference to this
	*
	* \param abstractSocket AbstractSocket to move in this
	*/

	AbstractSocket& AbstractSocket::operator=(AbstractSocket&& abstractSocket)
	{
		Close();

		m_handle = abstractSocket.m_handle;
		m_protocol = abstractSocket.m_protocol;
		m_isBlockingEnabled = abstractSocket.m_isBlockingEnabled;
		m_lastError = abstractSocket.m_lastError;
		m_state = abstractSocket.m_state;
		m_type = abstractSocket.m_type;
		
		abstractSocket.m_handle = SocketImpl::InvalidHandle;

		return *this;
	}
}

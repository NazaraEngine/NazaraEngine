// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Network/TcpClient.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
 #include <NazaraUtils/CallOnExit.hpp>
#include <limits>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/SocketImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/SocketImpl.hpp>
#else
#error Missing implementation: Socket
#endif


namespace Nz
{
	/*!
	* \ingroup network
	* \class Nz::TcpClient,l 
	* \brief Network class that represents a client in a TCP connection
	*/

	/*!
	* \brief Connects to the IpAddress
	* \return State of the socket
	*
	* \param remoteAddress Address to connect to
	*
	* \remark Produces a NazaraAssert if remote is invalid
	* \remark Produces a NazaraAssert if remote's port is not specified
	*/

	SocketState TcpClient::Connect(const IpAddress& remoteAddress)
	{
		NazaraAssert(remoteAddress.IsValid(), "Invalid remote address");
		NazaraAssert(remoteAddress.GetPort() != 0, "Remote address has no port");

		Disconnect();
		Open(remoteAddress.GetProtocol());

		CallOnExit restoreBlocking([this] { SocketImpl::SetBlocking(m_handle, true); });
		if (m_isBlockingEnabled)
			SocketImpl::SetBlocking(m_handle, false);
		else
			restoreBlocking.Reset();

		SocketState state = SocketImpl::Connect(m_handle, remoteAddress, &m_lastError);
		m_peerAddress = (state != SocketState::NotConnected) ? remoteAddress : IpAddress::Invalid;

		UpdateState(state);
		return state;
	}


	/*!
	* \brief Connects to the hostname
	* \return State of the socket
	*
	* \param hostName Hostname of the remote
	* \param protocol Net protocol to use
	* \param service Specify the protocol used
	* \param error Optional argument to get the error
	*/

	SocketState TcpClient::Connect(const std::string& hostName, NetProtocol protocol, const std::string& service, ResolveError* error)
	{
		UpdateState(SocketState::Resolving);
		std::vector<HostnameInfo> results = IpAddress::ResolveHostname(protocol, hostName, service, error);
		if (results.empty())
		{
			m_lastError = SocketError::ResolveError;

			UpdateState(SocketState::NotConnected);
			return m_state;
		}

		IpAddress hostnameAddress;
		for (const HostnameInfo& result : results)
		{
			if (!result.address)
				continue;

			if (result.socketType != SocketType::TCP)
				continue;

			hostnameAddress = result.address;
			break; //< Take first valid address
		}

		return Connect(hostnameAddress);
	}

	/*!
	* \brief Enables low delay in emitting
	*
	* \param lowDelay Should low delay be used
	*
	* \remark This may produce lag
	*/

	void TcpClient::EnableLowDelay(bool lowDelay)
	{
		if (m_isLowDelayEnabled != lowDelay)
		{
			if (m_handle != SocketImpl::InvalidHandle)
				SocketImpl::SetNoDelay(m_handle, lowDelay, &m_lastError);

			m_isLowDelayEnabled = lowDelay;
		}
	}

	/*!
	* \brief Enables the keep alive flag
	*
	* \param keepAlive Should the connection be kept alive
	* \param msTime Time in milliseconds before expiration
	* \param msInterval Interval in milliseconds between two pings
	*/

	void TcpClient::EnableKeepAlive(bool keepAlive, UInt64 msTime, UInt64 msInterval)
	{
		if (m_isKeepAliveEnabled != keepAlive || m_keepAliveTime != msTime || m_keepAliveInterval != msInterval)
		{
			if (m_handle != SocketImpl::InvalidHandle)
				SocketImpl::SetKeepAlive(m_handle, keepAlive, msTime, msInterval, &m_lastError);

			m_isKeepAliveEnabled = keepAlive;
			m_keepAliveInterval = msInterval;
			m_keepAliveTime = msTime;
		}
	}

	/*!
	* \brief Gets the size of the raw memory available
	* \return Size of the memory available
	*/

	UInt64 TcpClient::GetSize() const
	{
		return QueryAvailableBytes();
	}

	/*!
	* \brief Polls the connection status of the currently connecting socket
	* \return New socket state, which maybe unchanged (if connecting is still pending), SocketState::Connected if connection is successful or SocketState::NotConnected if connection failed
	*
	* This functions checks if the pending connection has either succeeded, failed or is still processing at the time of the call.
	*
	* \remark This function doesn't do anything if the socket is not currently connecting.
	*
	* \see WaitForConnected
	*/
	SocketState TcpClient::PollForConnected(UInt64 waitDuration)
	{
		switch (m_state)
		{
			case SocketState::Connecting:
			{
				NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");

				SocketState newState = SocketImpl::PollConnection(m_handle, m_peerAddress, waitDuration, &m_lastError);

				// Prevent valid peer address in non-connected state
				if (newState == SocketState::NotConnected)
				{
					m_openMode = OpenMode::NotOpen;
					m_peerAddress = IpAddress::Invalid;
				}

				UpdateState(newState);
				return newState;
			}

			case SocketState::Connected:
			case SocketState::NotConnected:
				return m_state;

			case SocketState::Bound:
			case SocketState::Resolving:
				break;
		}

		NazaraInternalError("unexpected socket state ({0:#x})", UnderlyingCast(m_state));
		return m_state;
	}

	/*!
	* \brief Receives the data available
	* \return true If data received
	*
	* \param buffer Raw memory to write
	* \param size Size of the buffer
	* \param received Optional argument to get the number of bytes received
	*
	* \remark Produces a NazaraAssert if socket is invalid
	* \remark Produces a NazaraAssert if buffer and its size is invalid
	*/

	bool TcpClient::Receive(void* buffer, std::size_t size, std::size_t* received)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");
		NazaraAssert(buffer && size > 0, "Invalid buffer");

		int read;
		if (!SocketImpl::Receive(m_handle, buffer, static_cast<int>(size), &read, &m_lastError))
		{
			switch (m_lastError)
			{
				case SocketError::ConnectionClosed:
				case SocketError::ConnectionRefused:
					UpdateState(SocketState::NotConnected);
					break;

				default:
					break;
			}

			return false;
		}

		if (received)
			*received = read;

		UpdateState(SocketState::Connected);
		return true;
	}

	/*!
	* \brief Sends the data available
	* \return true If data sended
	*
	* \param buffer Raw memory to read
	* \param size Size of the buffer
	* \param sent Optional argument to get the number of bytes sent
	*
	* \remark Large sending are handled, you do not need to call this multiple time
	* \remark Produces a NazaraAssert if socket is invalid
	* \remark Produces a NazaraAssert if buffer and its size is invalid
	*/
	bool TcpClient::Send(const void* buffer, std::size_t size, std::size_t* sent)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");
		NazaraAssert(buffer && size > 0, "Invalid buffer");

		std::size_t totalByteSent = 0;

		CallOnExit updateSent([sent, &totalByteSent]
		{
			*sent = totalByteSent;
		});

		if (!sent)
			updateSent.Reset();

		while (totalByteSent < size || !IsBlockingEnabled())
		{
			int sendSize = static_cast<int>(std::min<std::size_t>(size - totalByteSent, std::numeric_limits<int>::max())); //< Handle very large send
			int sentSize;
			if (!SocketImpl::Send(m_handle, static_cast<const UInt8*>(buffer) + totalByteSent, sendSize, &sentSize, &m_lastError))
			{
				switch (m_lastError)
				{
					case SocketError::ConnectionClosed:
					case SocketError::ConnectionRefused:
						UpdateState(SocketState::NotConnected);
						break;

					default:
						break;
				}

				return false;
			}

			totalByteSent += sentSize;
		}

		UpdateState(SocketState::Connected);
		return true;
	}

	/*!
	* \brief Sends multiple buffers at once
	* \return true If data were sent
	*
	* \param buffers A pointer to an array of NetBuffer containing buffers and size data
	* \param size Number of NetBuffer to send
	* \param sent Optional argument to get the number of bytes sent
	*/
	bool TcpClient::SendMultiple(const NetBuffer* buffers, std::size_t bufferCount, std::size_t* sent)
	{
		NazaraAssert(buffers && bufferCount > 0, "Invalid buffer");

		int byteSent;
		if (!SocketImpl::SendMultiple(m_handle, buffers, bufferCount, m_peerAddress, &byteSent, &m_lastError))
		{
			switch (m_lastError)
			{
				case SocketError::ConnectionClosed:
				case SocketError::ConnectionRefused:
					UpdateState(SocketState::NotConnected);
					break;

				default:
					break;
			}

			if (sent)
				*sent = byteSent;

			return false;
		}

		if (sent)
			*sent = byteSent;

		UpdateState(SocketState::Connected);
		return true;
	}

	/*!
	* \brief Waits for being connected before time out
	* \return The new socket state, either Connected if connection did succeed or NotConnected if an error occurred
	*
	* This functions waits for the pending connection to either succeed or fail for a specific duration before failing.
	*
	* \param msTimeout Time in milliseconds before time out (0 for system-specific duration, like a blocking connect would)
	*
	* \remark This function doesn't do anything if the socket is not currently connecting.
	*
	* \see PollForConnected
	*/
	SocketState TcpClient::WaitForConnected(UInt64 msTimeout)
	{
		switch (m_state)
		{
			case SocketState::Connecting:
			{
				NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");

				SocketState newState = SocketImpl::PollConnection(m_handle, m_peerAddress, (msTimeout > 0) ? msTimeout : std::numeric_limits<UInt64>::max(), &m_lastError);

				// If connection is still pending after waiting, cancel it
				if (newState == SocketState::Connecting)
					newState = SocketState::NotConnected;

				// Prevent valid data in non-connected state
				if (newState == SocketState::NotConnected)
				{
					m_openMode = OpenMode::NotOpen;
					m_peerAddress = IpAddress::Invalid;
				}

				UpdateState(newState);
				return newState;
			}

			case SocketState::Connected:
			case SocketState::NotConnected:
				return m_state;

			case SocketState::Bound:
			case SocketState::Resolving:
				break;
		}

		NazaraInternalError("unhandled socket state ({0:#x})", UnderlyingCast(m_state));
		return m_state;
	}

	/*!
	* \brief Flushes the stream
	*/

	void TcpClient::FlushStream()
	{
	}

	/*!
	* \brief Operation to do when closing socket
	*/

	void TcpClient::OnClose()
	{
		AbstractSocket::OnClose();

		m_openMode = OpenMode::NotOpen;
		m_peerAddress = IpAddress::Invalid;
	}

	/*!
	* \brief Operation to do when opening socket
	*
	* \remark Produces a NazaraWarning if delay mode or keep alive failed
	*/

	void TcpClient::OnOpened()
	{
		AbstractSocket::OnOpened();

		SocketError errorCode;

		if (!SocketImpl::SetNoDelay(m_handle, m_isLowDelayEnabled, &errorCode))
			NazaraWarning("failed to set socket no delay mode ({0:#x})", UnderlyingCast(errorCode));

		if (!SocketImpl::SetKeepAlive(m_handle, m_isKeepAliveEnabled, m_keepAliveTime, m_keepAliveInterval, &errorCode))
			NazaraWarning("failed to set socket keep alive mode ({0:#x})", UnderlyingCast(errorCode));

		m_peerAddress = IpAddress::Invalid;
		m_openMode = OpenMode_ReadWrite;
	}

	/*!
	* \brief Reads blocks
	* \return Number of blocks read
	*
	* \param buffer Preallocated buffer to contain information read
	* \param size Size of the read and thus of the buffer
	*
	* \remark Produces a NazaraAssert if socket is invalid
	*/

	std::size_t TcpClient::ReadBlock(void* buffer, std::size_t size)
	{
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");

		CallOnExit restoreBlocking([this] { SocketImpl::SetBlocking(m_handle, true); });
		if (m_isBlockingEnabled)
			SocketImpl::SetBlocking(m_handle, false);
		else
			restoreBlocking.Reset();

		std::size_t received;
		if (!Receive(buffer, size, &received))
			received = 0;

		return received;
	}

	/*!
	* \brief Resets the connection with a new socket and a peer address
	*
	* \param handle Socket to connect
	* \param peerAddress Address to connect to
	*/

	void TcpClient::Reset(SocketHandle handle, const IpAddress& peerAddress)
	{
		Open(handle);
		m_peerAddress = peerAddress;
		m_openMode = OpenMode_ReadWrite;
		UpdateState(SocketState::Connected);
	}

	/*!
	* \brief Sets the position of the cursor
	* \return false
	*
	* \param offset Offset according to the beginning of the stream
	*
	* \remark Produces a NazaraError because it is a special stream
	*/
	bool TcpClient::SeekStreamCursor(UInt64 /*offset*/)
	{
		NazaraError("SeekStreamCursor() cannot be used on sequential streams");
		return false;
	}

	/*!
	* \brief Gets the position of the cursor
	* \return 0
	*
	* \remark Produces a NazaraError because it is a special stream
	*/
	UInt64 TcpClient::TellStreamCursor() const
	{
		NazaraError("TellStreamCursor() cannot be used on sequential streams");
		return 0;
	}

	/*!
	* \brief Checks whether the stream reached the end of the stream
	* \return true if there is no more available bytes
	*/
	bool TcpClient::TestStreamEnd() const
	{
		return QueryAvailableBytes() == 0;
	}

	/*!
	* \brief Writes blocks
	* \return Number of blocks written
	*
	* \param buffer Preallocated buffer containing information to write
	* \param size Size of the writting and thus of the buffer
	*
	* \remark Produces a NazaraAssert if buffer is nullptr
	* \remark Produces a NazaraAssert if socket is invalid
	*/
	std::size_t TcpClient::WriteBlock(const void* buffer, std::size_t size)
	{
		NazaraAssert(buffer, "Invalid buffer");
		NazaraAssert(m_handle != SocketImpl::InvalidHandle, "Invalid handle");

		CallOnExit restoreBlocking([this] { SocketImpl::SetBlocking(m_handle, true); });
		if (m_isBlockingEnabled)
			SocketImpl::SetBlocking(m_handle, false);
		else
			restoreBlocking.Reset();

		std::size_t sent;
		if (!Send(buffer, size, &sent))
			sent = 0;

		return sent;
	}
}

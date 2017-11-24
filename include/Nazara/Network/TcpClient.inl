// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a TcpClient object by default
	*/

	inline TcpClient::TcpClient() :
	AbstractSocket(SocketType_TCP),
	Stream(StreamOption_Sequential),
	m_keepAliveInterval(1000),   //TODO: Query OS default value
	m_keepAliveTime(7'200'000),  //TODO: Query OS default value
	m_isKeepAliveEnabled(false), //TODO: Query OS default value
	m_isLowDelayEnabled(false)   //TODO: Query OS default value
	{
	}

	/*!
	* \brief Disconnects the connection
	*
	* \see Close
	*/

	inline void TcpClient::Disconnect()
	{
		Close();
	}

	/*!
	* \brief Gets the interval between two keep alive pings
	* \return Interval in milliseconds between two pings
	*/

	inline UInt64 TcpClient::GetKeepAliveInterval() const
	{
		return m_keepAliveInterval;
	}

	/*!
	* \brief Gets the time before expiration of connection
	* \return Time in milliseconds before expiration
	*/

	inline UInt64 TcpClient::GetKeepAliveTime() const
	{
		return m_keepAliveTime;
	}

	/*!
	* \brief Gets the remote address
	* \return Address of peer
	*/

	inline IpAddress TcpClient::GetRemoteAddress() const
	{
		return m_peerAddress;
	}

	/*!
	* \brief Checks whether low delay is enabled
	* \return true If it is the case
	*/

	inline bool TcpClient::IsLowDelayEnabled() const
	{
		return m_isLowDelayEnabled;
	}

	/*!
	* \brief Checks whether the keep alive flag is enabled
	* \return true If it is the case
	*/

	inline bool TcpClient::IsKeepAliveEnabled() const
	{
		return m_isKeepAliveEnabled;
	}
}

#include <Nazara/Network/DebugOff.hpp>

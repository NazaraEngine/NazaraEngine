// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline TcpClient::TcpClient() :
	AbstractSocket(SocketType_TCP),
	Stream(StreamOption_Sequential),
	m_keepAliveInterval(1000),   //TODO: Query OS default value
	m_keepAliveTime(7'200'000),  //TODO: Query OS default value
	m_isKeepAliveEnabled(false), //TODO: Query OS default value
	m_isLowDelayEnabled(false)   //TODO: Query OS default value
	{
	}

	inline void TcpClient::Disconnect()
	{
		Close();
	}

	inline UInt64 TcpClient::GetKeepAliveInterval() const
	{
		return m_keepAliveInterval;
	}

	inline UInt64 TcpClient::GetKeepAliveTime() const
	{
		return m_keepAliveTime;
	}

	inline IpAddress TcpClient::GetRemoteAddress() const
	{
		return m_peerAddress;
	}

	inline bool TcpClient::IsLowDelayEnabled() const
	{
		return m_isLowDelayEnabled;
	}

	inline bool TcpClient::IsKeepAliveEnabled() const
	{
		return m_isKeepAliveEnabled;
	}
}

#include <Nazara/Network/DebugOff.hpp>

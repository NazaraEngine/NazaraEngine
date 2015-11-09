// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline TcpClient::TcpClient(TcpClient&& tcpClient) :
	TcpBase(std::move(tcpClient)),
	m_peerAddress(std::move(tcpClient.m_peerAddress))
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
}

#include <Nazara/Network/DebugOff.hpp>

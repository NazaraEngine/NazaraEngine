// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	inline TcpBase::TcpBase() :
	AbstractSocket(SocketType_TCP)
	{
	}

	inline TcpBase::TcpBase(TcpBase&& tcpBase) :
	AbstractSocket(std::move(tcpBase)),
	m_state(tcpBase.m_state),
	m_keepAliveInterval(tcpBase.m_keepAliveInterval),
	m_keepAliveTime(tcpBase.m_keepAliveTime),
	m_isLowDelayEnabled(tcpBase.m_isLowDelayEnabled),
	m_isKeepAliveEnabled(tcpBase.m_isKeepAliveEnabled)
	{
	}

	inline bool TcpBase::IsLowDelayEnabled() const
	{
		return m_isLowDelayEnabled;
	}

	inline bool TcpBase::IsKeepAliveEnabled() const
	{
		return m_isKeepAliveEnabled;
	}
}

#include <Nazara/Network/DebugOff.hpp>

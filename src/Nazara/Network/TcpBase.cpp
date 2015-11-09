// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/TcpBase.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <limits>
#include <Nazara/Network/Debug.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/SocketImpl.hpp>
#else
#error Missing implementation: Socket
#endif

namespace Nz
{
	void TcpBase::OnOpened()
	{
		AbstractSocket::OnOpened();

		m_isLowDelayEnabled = false;  //< Nagle's algorithm, is this enabled everywhere?
		m_isKeepAliveEnabled = false; //< default documentation value, OS can change this (TODO: Query OS default value)
		m_keepAliveInterval = 1000;   //< default documentation value, OS can change this (TODO: Query OS default value)
		m_keepAliveTime = 7200000;    //< default documentation value, OS can change this (TODO: Query OS default value)
		
		ChangeState(SocketState_NotConnected);
	}
}
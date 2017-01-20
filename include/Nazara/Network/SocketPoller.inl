// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/SocketPoller.hpp>
#include <utility>
#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Constructs a SocketPoller object with another one by move semantic
	*
	* \param socketPoller SocketPoller to move into this
	*/
	inline SocketPoller::SocketPoller(SocketPoller&& socketPoller) :
	m_impl(socketPoller.m_impl)
	{
		socketPoller.m_impl = nullptr;
	}

	/*!
	* \brief Moves the SocketPoller into this
	* \return A reference to this
	*
	* \param socketPoller SocketPoller to move in this
	*/
	inline SocketPoller& SocketPoller::operator=(SocketPoller&& socketPoller)
	{
		m_impl = socketPoller.m_impl;
		socketPoller.m_impl = nullptr;

		return *this;
	}
}

#include <Nazara/Network/DebugOff.hpp>

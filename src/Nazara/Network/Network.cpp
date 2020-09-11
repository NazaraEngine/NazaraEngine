// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Network.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Network/Config.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <Nazara/Network/RUdpConnection.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/SocketImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/SocketImpl.hpp>
#else
#error Missing implementation: Network
#endif

#include <stdexcept>

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup network
	* \class Nz::Network
	* \brief Network class that represents the module initializer of Network
	*/

	Network::Network() :
	ModuleBase("Network", this)
	{
		// Initialize module here
		if (!SocketImpl::Initialize())
			throw std::runtime_error("failed to initialize socket implementation");

		if (!NetPacket::Initialize())
			throw std::runtime_error("failed to initialize packets");

		if (!RUdpConnection::Initialize())
			throw std::runtime_error("failed to initialize RUDP protocol");
	}

	Network::~Network()
	{
		// Uninitialize module here
		RUdpConnection::Uninitialize();
		NetPacket::Uninitialize();
		SocketImpl::Uninitialize();
	}

	Network* Network::s_instance = nullptr;
}


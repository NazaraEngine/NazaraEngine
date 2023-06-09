// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Network.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Network/Config.hpp>
#include <Nazara/Network/CurlLibrary.hpp>
#include <Nazara/Network/NetPacket.hpp>
#include <Nazara/Network/WebService.hpp>
#include <NazaraUtils/CallOnExit.hpp>

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

	Network::Network(Config config) :
	ModuleBase("Network", this)
	{
		// Initialize module here
		if (!SocketImpl::Initialize())
			throw std::runtime_error("failed to initialize socket implementation");

		if (!NetPacket::Initialize())
			throw std::runtime_error("failed to initialize packets");

		if (config.webServices)
		{
			m_curlLibrary = std::make_unique<CurlLibrary>();
			if (!m_curlLibrary->Load())
				throw std::runtime_error("failed to initialize curl");
		}
	}

	Network::~Network()
	{
		// Uninitialize module here
		m_curlLibrary.reset();

		NetPacket::Uninitialize();
		SocketImpl::Uninitialize();
	}

	std::unique_ptr<WebService> Network::InstantiateWebService()
	{
		if (!m_curlLibrary)
		{
			std::unique_ptr<CurlLibrary> curlLibrary = std::make_unique<CurlLibrary>();
			if (!curlLibrary->Load())
				throw std::runtime_error("failed to initialize curl");

			m_curlLibrary = std::move(curlLibrary);
		}

		return std::make_unique<WebService>(*m_curlLibrary);
	}

	Network* Network::s_instance = nullptr;
}

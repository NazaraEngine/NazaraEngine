// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Network/Network.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Network/Export.hpp>
#include <Nazara/Network/WebService.hpp>
#include <NazaraUtils/CallOnExit.hpp>

#ifndef NAZARA_PLATFORM_WEB
#include <Nazara/Network/CurlLibrary.hpp>
#endif

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/SocketImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/SocketImpl.hpp>
#else
#error Missing implementation: Network
#endif

#include <stdexcept>


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

#ifndef NAZARA_PLATFORM_WEB
		if (config.webServices)
		{
			m_curlLibrary = std::make_unique<CurlLibrary>();
			if (!m_curlLibrary->Load())
				throw std::runtime_error("failed to initialize curl");
		}
#endif
	}

	Network::~Network()
	{
#ifndef NAZARA_PLATFORM_WEB
		m_curlLibrary.reset();
#endif

		SocketImpl::Uninitialize();
	}

	std::unique_ptr<WebService> Network::InstantiateWebService()
	{
#ifndef NAZARA_PLATFORM_WEB
		if (!m_curlLibrary)
		{
			std::unique_ptr<CurlLibrary> curlLibrary = std::make_unique<CurlLibrary>();
			if (!curlLibrary->Load())
				throw std::runtime_error("failed to initialize curl");

			m_curlLibrary = std::move(curlLibrary);
		}

		return std::make_unique<WebService>(*m_curlLibrary);
#else
		return std::make_unique<WebService>();
#endif
	}

	Network* Network::s_instance = nullptr;
}

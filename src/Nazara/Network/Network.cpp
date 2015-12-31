// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Network.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Network/Config.hpp>
#include <Nazara/Network/Debug.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Network/Win32/SocketImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <Nazara/Network/Posix/SocketImpl.hpp>
#else
#error Missing implementation: Network
#endif

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	bool Network::Initialize()
	{
		if (s_moduleReferenceCounter > 0)
		{
			s_moduleReferenceCounter++;
			return true; // Already initialized
		}

		// Initialize module dependencies
		if (!Core::Initialize())
		{
			NazaraError("Failed to initialize core module");
			return false;
		}

		s_moduleReferenceCounter++;

		CallOnExit onExit(Network::Uninitialize);

		// Initialize module here
		if (!SocketImpl::Initialize())
		{
			NazaraError("Failed to initialize socket implementation");
			return false;
		}

		onExit.Reset();

		NazaraNotice("Initialized: Network module");
		return true;
	}

	bool Network::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Network::Uninitialize()
	{
		if (s_moduleReferenceCounter != 1)
		{
			// Either the module is not initialized, either it was initialized multiple times
			if (s_moduleReferenceCounter > 1)
				s_moduleReferenceCounter--;

			return;
		}

		s_moduleReferenceCounter = 0;

		// Uninitialize module here
		SocketImpl::Uninitialize();

		NazaraNotice("Uninitialized: Network module");

		// Free module dependencies
		Core::Uninitialize();
	}

	unsigned int Network::s_moduleReferenceCounter = 0;
}


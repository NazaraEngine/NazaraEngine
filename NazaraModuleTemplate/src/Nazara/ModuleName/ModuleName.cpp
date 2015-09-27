// Copyright (C) YEAR AUTHORS
// This file is part of the "Nazara Engine - Module name"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ModuleName/ModuleName.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/ModuleName/Config.hpp>
#include <Nazara/ModuleName/Debug.hpp>

namespace Nz
{
	bool ModuleName::Initialize()
	{
		if (s_moduleReferenceCounter > 0)
		{
			s_moduleReferenceCounter++;
			return true; // Already initialized
		}

		// Initialize module dependencies
		if (!NzCore::Initialize())
		{
			NazaraError("Failed to initialize core module");
			return false;
		}

		s_moduleReferenceCounter++;

		CallOnExit onExit(ModuleName::Uninitialize);

		// Initialize module here

		onExit.Reset();

		NazaraNotice("Initialized: ModuleName module");
		return true;
	}

	bool ModuleName::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void ModuleName::Uninitialize()
	{
		if (s_moduleReferenceCounter != 1)
		{
			// Le module est soit encore utilisé, soit pas initialisé
			if (s_moduleReferenceCounter > 1)
				s_moduleReferenceCounter--;

			return;
		}

		s_moduleReferenceCounter = 0;

		// Uninitialize module here

		NazaraNotice("Uninitialized: ModuleName module");

		// Free module dependencies
		Core::Uninitialize();
	}

	unsigned int ModuleName::s_moduleReferenceCounter = 0;	
}


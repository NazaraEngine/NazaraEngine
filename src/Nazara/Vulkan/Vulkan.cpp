// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/Vulkan.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	bool Vulkan::Initialize()
	{
		if (s_moduleReferenceCounter > 0)
		{
			s_moduleReferenceCounter++;
			return true; // Already initialized
		}

		// Initialize module dependencies
		if (!Utility::Initialize())
		{
			NazaraError("Failed to initialize utility module");
			return false;
		}

		s_moduleReferenceCounter++;

		CallOnExit onExit(Vulkan::Uninitialize);

		// Initialize module here

		onExit.Reset();

		NazaraNotice("Initialized: Vulkan module");
		return true;
	}

	bool Vulkan::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Vulkan::Uninitialize()
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

		NazaraNotice("Uninitialized: Vulkan module");

		// Free module dependencies
		Utility::Uninitialize();
	}

	unsigned int Vulkan::s_moduleReferenceCounter = 0;	
}


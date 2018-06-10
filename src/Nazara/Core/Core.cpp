// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/PluginManager.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Core
	* \brief Core class that represents the module initializer of Core
	*/

	/*!
	* \brief Initializes the Core module
	* \return true if initialization is successful
	*
	* \remark Produces a NazaraNotice
	*/

	bool Core::Initialize()
	{
		if (IsInitialized())
		{
			s_moduleReferenceCounter++;
			return true; // Already initialized
		}

		s_moduleReferenceCounter++;

		Log::Initialize();

		NazaraNotice("Initialized: Core");
		return true;
	}

	/*!
	* \brief Checks whether the module is initialized
	* \return true if module is initialized
	*/

	bool Core::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	/*!
	* \brief Uninitializes the Core module
	*
	* \remark Produces a NazaraNotice
	*/

	void Core::Uninitialize()
	{
		if (s_moduleReferenceCounter != 1)
		{
			// The module is still in use, or can not be uninitialized
			if (s_moduleReferenceCounter > 1)
				s_moduleReferenceCounter--;

			return;
		}

		// Free of module
		s_moduleReferenceCounter = 0;

		HardwareInfo::Uninitialize();
		Log::Uninitialize();
		PluginManager::Uninitialize();
		TaskScheduler::Uninitialize();

		NazaraNotice("Uninitialized: Core");
	}

	unsigned int Core::s_moduleReferenceCounter = 0;
}

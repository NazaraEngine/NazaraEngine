// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Platform module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Platform/Config.hpp>
#include <Nazara/Platform/Cursor.hpp>
#include <Nazara/Platform/Window.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <Nazara/Platform/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup system
	* \class Nz::Platform
	* \brief Platform class that represents the module initializer of Platform
	*/

	/*!
	* \brief Initializes the Platform module
	* \return true if initialization is successful
	*
	* \remark Produces a NazaraNotice
	* \remark Produces a NazaraError if one submodule failed
	*/

	bool Platform::Initialize()
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

		// Initialisation of the module
		CallOnExit onExit(Platform::Uninitialize);

		if (!Window::Initialize())
		{
			NazaraError("Failed to initialize window's system");
			return false;
		}

		// Must be initialized after Window
		if (!Cursor::Initialize())
		{
			NazaraError("Failed to initialize cursors");
			return false;
		}

		onExit.Reset();

		NazaraNotice("Initialized: Platform module");
		return true;
	}

	/*!
	* \brief Checks whether the module is initialized
	* \return true if module is initialized
	*/

	bool Platform::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	/*!
	* \brief Uninitializes the Platform module
	*
	* \remark Produces a NazaraNotice
	*/

	void Platform::Uninitialize()
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

		Cursor::Uninitialize(); //< Must be done before Window
		Window::Uninitialize();

		NazaraNotice("Uninitialized: Platform module");

		// Free of dependances
		Utility::Uninitialize();
	}

	unsigned int Platform::s_moduleReferenceCounter = 0;
}

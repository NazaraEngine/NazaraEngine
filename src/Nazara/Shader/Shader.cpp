// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Shader generator"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/Shader.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Shader/Config.hpp>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	bool Shader::Initialize()
	{
		if (s_moduleReferenceCounter > 0)
		{
			s_moduleReferenceCounter++;
			return true; // Already initialized
		}

		// Initialize module dependencies
		if (!Core::Initialize())
		{
			NazaraError("Failed to initialize shader module");
			return false;
		}

		s_moduleReferenceCounter++;

		CallOnExit onExit(Shader::Uninitialize);

		// Initialize module here

		onExit.Reset();

		NazaraNotice("Initialized: Shader module");
		return true;
	}

	bool Shader::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Shader::Uninitialize()
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

		NazaraNotice("Uninitialized: Shader module");

		// Free module dependencies
		Core::Uninitialize();
	}

	unsigned int Shader::s_moduleReferenceCounter = 0;	
}


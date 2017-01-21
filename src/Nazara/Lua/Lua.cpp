// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/Lua.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Lua/Config.hpp>
#include <Nazara/Lua/Debug.hpp>

namespace Nz
{
	bool Lua::Initialize()
	{
		if (s_moduleReferenceCounter > 0)
		{
			s_moduleReferenceCounter++;
			return true; // Déjà initialisé
		}

		// Initialisation des dépendances
		if (!Core::Initialize())
		{
			NazaraError("Failed to initialize core module");
			return false;
		}

		s_moduleReferenceCounter++;

		// Initialisation du module

		NazaraNotice("Initialized: Lua module");
		return true;
	}

	bool Lua::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Lua::Uninitialize()
	{
		if (s_moduleReferenceCounter != 1)
		{
			// Le module est soit encore utilisé, soit pas initialisé
			if (s_moduleReferenceCounter > 1)
				s_moduleReferenceCounter--;

			return;
		}

		// Libération du module
		s_moduleReferenceCounter = 0;

		NazaraNotice("Uninitialized: Lua module");

		// Libération des dépendances
		Core::Uninitialize();
	}

	unsigned int Lua::s_moduleReferenceCounter = 0;
}

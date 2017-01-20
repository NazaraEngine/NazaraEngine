// Copyright (C) 2017 Rémi Bèges
// This file is part of the "Nazara Engine - Noise module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Noise/Noise.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Noise/Config.hpp>
#include <Nazara/Noise/Debug.hpp>

namespace Nz
{
	bool Noise::Initialize()
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
			Uninitialize();

			return false;
		}

		s_moduleReferenceCounter++;

		// Initialisation du module

		NazaraNotice("Initialized: Noise module");
		return true;
	}

	bool Noise::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Noise::Uninitialize()
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

		NazaraNotice("Uninitialized: Noise module");

		// Libération des dépendances
		Core::Uninitialize();
	}

	unsigned int Noise::s_moduleReferenceCounter = 0;
}

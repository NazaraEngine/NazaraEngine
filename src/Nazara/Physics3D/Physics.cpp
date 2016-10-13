// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics/Physics.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Physics/Config.hpp>
#include <Nazara/Physics/Geom.hpp>
#include <Newton/Newton.h>
#include <Nazara/Physics/Debug.hpp>

namespace Nz
{
	unsigned int Physics::GetMemoryUsed()
	{
		return NewtonGetMemoryUsed();
	}

	bool Physics::Initialize()
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
		if (!PhysGeom::Initialize())
		{
			NazaraError("Failed to initialize geoms");
			return false;
		}

		NazaraNotice("Initialized: Physics module");
		return true;
	}

	bool Physics::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Physics::Uninitialize()
	{
		if (s_moduleReferenceCounter != 1)
		{
			// Le module est soit encore utilisé, soit pas initialisé
			if (s_moduleReferenceCounter > 1)
				s_moduleReferenceCounter--;

			return;
		}

		// Libération du module
		PhysGeom::Uninitialize();

		s_moduleReferenceCounter = 0;

		NazaraNotice("Uninitialized: Physics module");

		// Libération des dépendances
		Core::Uninitialize();
	}

	unsigned int Physics::s_moduleReferenceCounter = 0;
}

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Physics3D.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <Nazara/Physics3D/Collider3D.hpp>
#include <Newton/Newton.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	unsigned int Physics3D::GetMemoryUsed()
	{
		return NewtonGetMemoryUsed();
	}

	bool Physics3D::Initialize()
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
		if (!Collider3D::Initialize())
		{
			NazaraError("Failed to initialize geoms");
			return false;
		}

		NazaraNotice("Initialized: Physics3D module");
		return true;
	}

	bool Physics3D::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Physics3D::Uninitialize()
	{
		if (s_moduleReferenceCounter != 1)
		{
			// Le module est soit encore utilisé, soit pas initialisé
			if (s_moduleReferenceCounter > 1)
				s_moduleReferenceCounter--;

			return;
		}

		// Libération du module
		Collider3D::Uninitialize();

		s_moduleReferenceCounter = 0;

		NazaraNotice("Uninitialized: Physics3D module");

		// Libération des dépendances
		Core::Uninitialize();
	}

	unsigned int Physics3D::s_moduleReferenceCounter = 0;
}

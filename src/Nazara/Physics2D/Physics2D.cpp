// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	bool Physics2D::Initialize()
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

		NazaraNotice("Initialized: Physics2D module");
		return true;
	}

	bool Physics2D::IsInitialized()
	{
		return s_moduleReferenceCounter != 0;
	}

	void Physics2D::Uninitialize()
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

		NazaraNotice("Uninitialized: Physics2D module");

		// Libération des dépendances
		Core::Uninitialize();
	}

	unsigned int Physics2D::s_moduleReferenceCounter = 0;
}

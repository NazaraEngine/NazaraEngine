// Copyright (C) 2013 AUTHORS
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/Debug.hpp>

bool NzCore::Initialize()
{
	if (s_moduleReferenceCounter++ != 0)
		return true; // Déjà initialisé

	NazaraNotice("Initialized: Core");

	return true;
}

bool NzCore::IsInitialized()
{
	return s_moduleReferenceCounter != 0;
}

void NzCore::Uninitialize()
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

	NzHardwareInfo::Uninitialize();
	NzTaskScheduler::Uninitialize();

	NazaraNotice("Uninitialized: Core");
}

unsigned int NzCore::s_moduleReferenceCounter = 0;

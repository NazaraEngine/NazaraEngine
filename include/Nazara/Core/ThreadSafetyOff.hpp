// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Pas de header guard

// Ces macros peuvent changer pour n'importe quel fichier qui l'utilise dans une même unité de compilation
#undef NazaraLock
#undef NazaraMutex
#undef NazaraMutexAttrib
#undef NazaraMutexLock
#undef NazaraMutexUnlock
#undef NazaraNamedLock

#define NazaraLock(mutex)
#define NazaraMutex(name)
#define NazaraMutexAttrib(name, attribute)
#define NazaraMutexLock(mutex)
#define NazaraMutexUnlock(mutex)
#define NazaraNamedLock(mutex, name)


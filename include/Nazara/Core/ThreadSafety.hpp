// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Pas de header guard

#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/Mutex.hpp>

// Ces macros peuvent changer pour n'importe quel fichier qui l'utilise dans une même unité de compilation
#undef NazaraLock
#undef NazaraMutex
#undef NazaraMutexAttrib
#undef NazaraMutexLock
#undef NazaraMutexUnlock
#undef NazaraNamedLock

#define NazaraLock(mutex) NzLockGuard lock_mutex(mutex);
#define NazaraMutex(name) NzMutex name;
#define NazaraMutexAttrib(name, attribute) attribute NzMutex name;
#define NazaraMutexLock(mutex) mutex.Lock();
#define NazaraMutexUnlock(mutex) mutex.Unlock();
#define NazaraNamedLock(mutex, name) NzLockGuard lock_##name(mutex);

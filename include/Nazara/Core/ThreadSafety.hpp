// Copyright (C) 2015 Jérôme Leclercq
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

#define NazaraLock(mutex) Nz::LockGuard lock_mutex(mutex);
#define NazaraMutex(name) Nz::Mutex name;
#define NazaraMutexAttrib(name, attribute) attribute Mutex name;
#define NazaraMutexLock(mutex) mutex.Lock();
#define NazaraMutexUnlock(mutex) mutex.Unlock();
#define NazaraNamedLock(mutex, name) Nz::LockGuard lock_##name(mutex);

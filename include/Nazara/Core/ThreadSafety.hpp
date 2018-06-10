// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// No header guard

#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/Mutex.hpp>

// These macroes can change for any file which uses it in the same unit of compilation
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

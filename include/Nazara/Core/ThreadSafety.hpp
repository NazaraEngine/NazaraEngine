// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Pas de header guard

#include <Nazara/Core/Config.hpp>

// Ces macros peuvent changer pour n'importe quel fichier qui l'utilise dans une même unité de compilation
#undef NazaraLock
#undef NazaraMutex
#undef NazaraMutexAttrib
#undef NazaraMutexLock
#undef NazaraMutexUnlock
#undef NazaraNamedLock

#if NAZARA_CORE_THREADSAFE && (\
	(NAZARA_THREADSAFETY_BYTEARRAY	  && defined(NAZARA_CLASS_BYTEARRAY)) || \
	(NAZARA_THREADSAFETY_CLOCK		  && defined(NAZARA_CLASS_CLOCK)) || \
	(NAZARA_THREADSAFETY_DIRECTORY	  && defined(NAZARA_CLASS_DIRECTORY)) || \
	(NAZARA_THREADSAFETY_DYNLIB		  && defined(NAZARA_CLASS_DYNLIB)) || \
	(NAZARA_THREADSAFETY_FILE		  && defined(NAZARA_CLASS_FILE)) || \
	(NAZARA_THREADSAFETY_LOG		  && defined(NAZARA_CLASS_LOG)) || \
	(NAZARA_THREADSAFETY_STRING		  && defined(NAZARA_CLASS_STRING)) || \
	(NAZARA_THREADSAFETY_STRINGSTREAM && defined(NAZARA_CLASS_STRINGSTREAM)))

	#include <Nazara/Core/LockGuard.hpp>
	#include <Nazara/Core/Mutex.hpp>

	#define NazaraLock(mutex) NzLockGuard lock_mutex(mutex);
	#define NazaraMutex(name) NzMutex name;
	#define NazaraMutexAttrib(name, attribute) attribute NzMutex name;
	#define NazaraMutexLock(mutex) mutex.Lock();
	#define NazaraMutexUnlock(mutex) mutex.Unlock();
	#define NazaraNamedLock(mutex, name) NzLock lock_##name(mutex);
#else
	#define NazaraLock(mutex)
	#define NazaraMutex(name)
	#define NazaraMutexAttrib(name, attribute)
	#define NazaraMutexLock(mutex)
	#define NazaraMutexUnlock(mutex)
	#define NazaraNamedLock(mutex, name)
#endif

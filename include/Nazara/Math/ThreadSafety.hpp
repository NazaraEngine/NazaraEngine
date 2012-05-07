// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Pas de header guard

#include <Nazara/Math/Config.hpp>

// Ces macros peuvent changer pour n'importe quel fichier qui l'utilise, dans une même unité de compilation
#undef NazaraLock
#undef NazaraMutex
#undef NazaraMutexAttrib
#undef NazaraMutexLock
#undef NazaraMutexUnlock
#undef NazaraNamedLock

#if NAZARA_MATH_THREADSAFE && (\
	(NAZARA_THREADSAFETY_MATRIX3      && (defined(NAZARA_MATRIX3)      || defined(NAZARA_MATRIX3_CPP))) || \
	(NAZARA_THREADSAFETY_MATRIX4      && (defined(NAZARA_MATRIX4)      || defined(NAZARA_MATRIX4_CPP))))

	#include <Nazara/Core/Lock.hpp>
	#include <Nazara/Core/Mutex.hpp>

	#define NazaraLock(mutex) NzLock lock_mutex(mutex);
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

// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Pas de header guard

#include <Nazara/Core/Config.hpp>

#undef NazaraLock
#undef NazaraMutex
#undef NazaraMutexAttrib
#undef NazaraMutexLock
#undef NazaraMutexUnlock
#undef NazaraNamedLock

#if NAZARA_CORE_THREADSAFE && (\
	(NAZARA_THREADSAFETY_BYTEARRAY	  && (defined(NAZARA_BYTEARRAY)	   || defined(NAZARA_BYTEARRAY_CPP))) || \
	(NAZARA_THREADSAFETY_CLOCK		  && (defined(NAZARA_CLOCK)		   || defined(NAZARA_CLOCK_CPP))) || \
	(NAZARA_THREADSAFETY_DIRECTORY	  && (defined(NAZARA_DIRECTORY)	   || defined(NAZARA_DIRECTORY_CPP))) || \
	(NAZARA_THREADSAFETY_DYNLIB		  && (defined(NAZARA_DYNLIB)	   || defined(NAZARA_DYNLIB_CPP))) || \
	(NAZARA_THREADSAFETY_FILE		  && (defined(NAZARA_FILE)		   || defined(NAZARA_FILE_CPP))) || \
	(NAZARA_THREADSAFETY_HASHDIGEST	  && (defined(NAZARA_HASHDIGEST)   || defined(NAZARA_HASHDIGEST_CPP))) || \
	(NAZARA_THREADSAFETY_LOG		  && (defined(NAZARA_LOG)		   || defined(NAZARA_LOG_CPP))) || \
	(NAZARA_THREADSAFETY_STRING		  && (defined(NAZARA_STRING)	   || defined(NAZARA_STRING_CPP))) || \
	(NAZARA_THREADSAFETY_STRINGSTREAM && (defined(NAZARA_STRINGSTREAM) || defined(NAZARA_STRINGSTREAM_CPP))))

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

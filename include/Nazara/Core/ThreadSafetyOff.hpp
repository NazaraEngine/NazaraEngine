// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// No header guard

// These macroes can change for any file which uses it in the same unit of compilation
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


// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/ClockImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <ctime>
#include <windows.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
		LARGE_INTEGER s_frequency; // La fréquence ne varie pas pas au cours de l'exécution
	}

	bool ClockImplInitializeHighPrecision()
	{
		return QueryPerformanceFrequency(&s_frequency) != 0;
	}

	UInt64 ClockImplGetElapsedMicroseconds()
	{
		// http://msdn.microsoft.com/en-us/library/windows/desktop/ms644904(v=vs.85).aspx
		//HANDLE thread = GetCurrentThread();
		//DWORD oldMask = SetThreadAffinityMask(thread, 1);

		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		//SetThreadAffinityMask(thread, oldMask);

		return time.QuadPart*1000000ULL / s_frequency.QuadPart;
	}

	UInt64 ClockImplGetElapsedMilliseconds()
	{
		#ifdef NAZARA_PLATFORM_WINDOWS_VISTA
		return GetTickCount64();
		#else
		return GetTickCount();
		#endif
	}
}

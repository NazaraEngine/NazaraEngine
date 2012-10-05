// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/ClockImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <ctime>
#include <windows.h>
#include <Nazara/Core/Debug.hpp>

namespace
{
	LARGE_INTEGER frequency; // La fréquence ne varie pas pas au cours de l'exécution
}

bool NzClockImplInitializeHighPrecision()
{
	return QueryPerformanceFrequency(&frequency) != 0;
}

nzUInt64 NzClockImplGetMicroseconds()
{
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms644904(v=vs.85).aspx
	HANDLE thread = GetCurrentThread();
	DWORD oldMask = SetThreadAffinityMask(thread, 1);

	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);

	SetThreadAffinityMask(thread, oldMask);

	return time.QuadPart * 1000000ULL / frequency.QuadPart;
}

nzUInt64 NzClockImplGetMilliseconds()
{
	#ifdef NAZARA_PLATFORM_WINDOWS_VISTA
	return GetTickCount64();
	#else
	return GetTickCount();
	#endif
}

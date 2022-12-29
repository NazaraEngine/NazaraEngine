// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Time.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/TimeImpl.hpp>
#elif defined(NAZARA_PLATFORM_MACOS) || defined(NAZARA_PLATFORM_IOS)
	#include <Nazara/Core/Darwin/TimeImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/TimeImpl.hpp>
#else
	#error OS not handled
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		Time GetElapsedNanosecondsFirstRun()
		{
			if (InitializeHighPrecisionTimer())
				GetElapsedNanoseconds = GetElapsedNanosecondsImpl;
			else
				GetElapsedNanoseconds = GetElapsedMillisecondsImpl;

			return GetElapsedNanoseconds();
		}
	}

	GetElapsedTimeFunction GetElapsedMilliseconds = GetElapsedMillisecondsImpl;
	GetElapsedTimeFunction GetElapsedNanoseconds = NAZARA_ANONYMOUS_NAMESPACE_PREFIX(GetElapsedNanosecondsFirstRun);
}

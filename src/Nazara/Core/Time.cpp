// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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
			if (PlatformImpl::InitializeHighPrecisionTimer())
				GetElapsedNanoseconds = PlatformImpl::GetElapsedNanosecondsImpl;
			else
				GetElapsedNanoseconds = PlatformImpl::GetElapsedMillisecondsImpl;

			return GetElapsedNanoseconds();
		}
	}

	GetElapsedTimeFunction GetElapsedMilliseconds = PlatformImpl::GetElapsedMillisecondsImpl;
	GetElapsedTimeFunction GetElapsedNanoseconds = NAZARA_ANONYMOUS_NAMESPACE_PREFIX(GetElapsedNanosecondsFirstRun);
}

// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Time.hpp>
#include <cstdlib>
#include <ostream>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/TimeImpl.hpp>
#elif defined(NAZARA_PLATFORM_MACOS) || defined(NAZARA_PLATFORM_IOS)
	#include <Nazara/Core/Darwin/TimeImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/TimeImpl.hpp>
#else
	#error OS not handled
#endif


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

	std::ostream& operator<<(std::ostream& out, Time time)
	{
		Int64 ns = time.AsNanoseconds();
		Int64 nsAbs = std::llabs(ns);

		if (nsAbs > 1'000'000'000)
			return out << time.AsSeconds<double>() << "s";
		else if (nsAbs > 1'000'000)
			return out << ns / 1'000'000.0 << "ms";
		else if (nsAbs > 1'000)
			return out << ns / 1'000.0 << "us";
		else
			return out << ns << "ns";
	}

	GetElapsedTimeFunction GetElapsedMilliseconds = PlatformImpl::GetElapsedMillisecondsImpl;
	GetElapsedTimeFunction GetElapsedNanoseconds = NAZARA_ANONYMOUS_NAMESPACE_PREFIX(GetElapsedNanosecondsFirstRun);
}

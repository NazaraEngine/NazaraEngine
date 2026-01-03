// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Darwin/TimeImpl.hpp>
#include <time.h>

namespace Nz::PlatformImpl
{
	bool InitializeHighPrecisionTimer()
	{
		return true; //< No initialization required
	}

	Time GetElapsedNanosecondsImpl()
	{
		UInt64 nanoseconds = clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW);
		return Time::Nanoseconds(static_cast<Int64>(nanoseconds));
	}

	Time GetElapsedMillisecondsImpl()
	{
		timespec time;
		clock_gettime(CLOCK_MONOTONIC, &time);

		return Time::Seconds(time.tv_sec) + Time::Nanoseconds(time.tv_nsec);
	}
}

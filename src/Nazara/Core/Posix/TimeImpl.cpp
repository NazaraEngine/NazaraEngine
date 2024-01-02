// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/TimeImpl.hpp>
#include <time.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	bool InitializeHighPrecisionTimer()
	{
		return true; //< No initialization required
	}

	Time GetElapsedNanosecondsImpl()
	{
		timespec time;
		clock_gettime(CLOCK_MONOTONIC, &time);

		return Time::Seconds(time.tv_sec) + Time::Nanoseconds(time.tv_nsec);
	}

	Time GetElapsedMillisecondsImpl()
	{
		timespec time;
		clock_gettime(CLOCK_MONOTONIC, &time);

		return Time::Seconds(time.tv_sec) + Time::Nanoseconds(time.tv_nsec);
	}
}

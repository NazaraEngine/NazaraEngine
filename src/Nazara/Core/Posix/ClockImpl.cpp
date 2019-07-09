// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/ClockImpl.hpp>
#include <sys/time.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	bool ClockImplInitializeHighPrecision()
	{
		return true; // No initialization needed
	}

	UInt64 ClockImplGetElapsedMicroseconds()
	{
		timeval clock;
		gettimeofday(&clock, nullptr);
		return static_cast<UInt64>(clock.tv_sec*1000000 + clock.tv_usec);
	}

	UInt64 ClockImplGetElapsedMilliseconds()
	{
		timeval clock;
		gettimeofday(&clock, nullptr);
		return static_cast<UInt64>(clock.tv_sec*1000 + (clock.tv_usec/1000));
	}
}

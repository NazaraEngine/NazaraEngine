// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Timestamp.hpp>
#include <ostream>

#if __cpp_lib_chrono < 201907L

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <sys/time.h>
#endif

#endif

namespace Nz
{
	Timestamp Timestamp::Now()
	{
#if __cpp_lib_chrono >= 201907L
		auto now = std::chrono::utc_clock::now();
		return FromTimepoint(now);
#else

// No standard support, use OS functions
#if defined(NAZARA_PLATFORM_WINDOWS)
		FILETIME time;
#if _WIN32_WINNT >= _WIN32_WINNT_WIN8
		::GetSystemTimePreciseAsFileTime(&time);
#else
		::GetSystemTimeAsFileTime(&time);
#endif
		Int64 nanoseconds = Int64(time.dwHighDateTime) << 32 | time.dwLowDateTime;

		// FILETIME is relative to Windows Epoch (January 1, 1601), convert to regular Epoch
		constexpr Int64 EpochDiff = 116444736000000000ll; //< nanoseconds difference between January 1, 1601 and January 1, 1970
		nanoseconds -= EpochDiff;

		// FILETIME stores the number of "100-nanosecond interval"
		nanoseconds *= 100;

		return FromNanoseconds(nanoseconds);
#elif defined(NAZARA_PLATFORM_POSIX)
		struct timeval tv;
		::gettimeofday(&tv, nullptr);

		return FromNanoseconds(tv.tv_sec * 1'000'000'000ll + tv.tv_usec);
#else
#error No fallback implementation for Timestamp::Now()
#endif

#endif
	}

	std::ostream& operator<<(std::ostream& out, Timestamp timestamp)
	{
		UInt64 seconds = timestamp.AsSeconds();
		Time remainder = timestamp.GetRemainder();
		out << "timestamp: " << seconds;
		if (remainder > Time::Microsecond())
			out << '.' << remainder.AsMilliseconds();

		return out;
	}
}

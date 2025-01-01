// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Win32/TimeImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <ctime>
#include <Windows.h>

namespace Nz::PlatformImpl
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		LARGE_INTEGER s_frequency; // The frequency of the performance counter is fixed at system boot and is consistent across all processors
	}

	bool InitializeHighPrecisionTimer()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		return QueryPerformanceFrequency(&s_frequency) != 0 && s_frequency.QuadPart != 0;
	}

	Time GetElapsedNanosecondsImpl()
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		if (s_frequency.QuadPart == 10'000'000) //< seems to be a common value
			return Time::Nanoseconds(100ll * time.QuadPart);
		else
		{
			// Compute using 128bits precisions
			// https://stackoverflow.com/questions/23378063/how-can-i-use-mach-absolute-time-without-overflowing

			UInt64 num = 1'000'000'000ll;
			UInt64 denom = s_frequency.QuadPart;
			UInt64 value = time.QuadPart;

			UInt64 high = (value >> 32) * num;
			UInt64 low = (value & 0xFFFFFFFFull) * num / denom;
			UInt64 highRem = ((high % denom) << 32) / denom;
			high /= denom;

			return Time::Nanoseconds(SafeCast<Int64>((high << 32) + highRem + low));
		}
	}

	Time GetElapsedMillisecondsImpl()
	{
#if NAZARAUTILS_WINDOWS_NT6
		return Time::Milliseconds(GetTickCount64());
#else
		return Time::Milliseconds(GetTickCount());
#endif
	}
}

#include <Nazara/Core/AntiWindows.hpp>

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/ClockImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/ClockImpl.hpp>
#else
	#error OS not handled
#endif

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_CLOCK
	#include <Nazara/Core/ThreadSafety.hpp>
#else
	#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		UInt64 GetMicrosecondsLowPrecision()
		{
			return ClockImplGetElapsedMilliseconds()*1000ULL;
		}

		UInt64 GetElapsedMicrosecondsFirstRun()
		{
			if (ClockImplInitializeHighPrecision())
				GetElapsedMicroseconds = ClockImplGetElapsedMicroseconds;
			else
				GetElapsedMicroseconds = GetMicrosecondsLowPrecision;

			return GetElapsedMicroseconds();
		}
	}

	Clock::Clock(UInt64 startingValue, bool paused) :
	m_elapsedTime(startingValue),
	m_refTime(GetElapsedMicroseconds()),
	m_paused(paused)
	{
	}

	float Clock::GetSeconds() const
	{
		return GetMicroseconds()/1000000.f;
	}

	UInt64 Clock::GetMicroseconds() const
	{
		NazaraLock(m_mutex);

		UInt64 elapsedMicroseconds = m_elapsedTime;
		if (!m_paused)
			elapsedMicroseconds += (GetElapsedMicroseconds() - m_refTime);

		return elapsedMicroseconds;
	}

	UInt64 Clock::GetMilliseconds() const
	{
		return GetMicroseconds()/1000;
	}

	bool Clock::IsPaused() const
	{
		NazaraLock(m_mutex);

		return m_paused;
	}

	void Clock::Pause()
	{
		NazaraLock(m_mutex);

		if (!m_paused)
		{
			m_elapsedTime += GetElapsedMicroseconds() - m_refTime;
			m_paused = true;
		}
		else
			NazaraWarning("Clock is already paused, ignoring...");
	}

	void Clock::Restart()
	{
		NazaraLock(m_mutex);

		m_elapsedTime = 0;
		m_refTime = GetElapsedMicroseconds();
		m_paused = false;
	}

	void Clock::Unpause()
	{
		NazaraLock(m_mutex);

		if (m_paused)
		{
			m_refTime = GetElapsedMicroseconds();
			m_paused = false;
		}
		else
			NazaraWarning("Clock is not paused, ignoring...");
	}

	ClockFunction GetElapsedMicroseconds = Detail::GetElapsedMicrosecondsFirstRun;
	ClockFunction GetElapsedMilliseconds = ClockImplGetElapsedMilliseconds;
}

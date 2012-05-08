// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
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

#define NAZARA_CLASS_CLOCK
#include <Nazara/Core/ThreadSafety.hpp>
#include <Nazara/Core/Debug.hpp>

NzClock::NzClock() :
m_elapsedTime(0),
m_refTime(NzGetMicroseconds()),
m_paused(false)
{
}

float NzClock::GetSeconds() const
{
	return GetMicroseconds()/1000000.f;
}

nzUInt64 NzClock::GetMicroseconds() const
{
	NazaraMutex(m_mutex);

	return m_elapsedTime + (NzGetMicroseconds()-m_refTime);
}

nzUInt64 NzClock::GetMilliseconds() const
{
	return GetMicroseconds()/1000;
}

bool NzClock::IsPaused() const
{
	NazaraMutex(m_mutex);

	return m_paused;
}

void NzClock::Pause()
{
	NazaraMutex(m_mutex);

	if (!m_paused)
	{
		m_elapsedTime += NzGetMicroseconds()-m_refTime;
		m_paused = true;
	}
	else
		NazaraWarning("Clock is already paused, ignoring...");
}

void NzClock::Restart()
{
	NazaraMutex(m_mutex);

	m_elapsedTime = 0;
	m_refTime = NzGetMicroseconds();
	m_paused = false;
}

void NzClock::Unpause()
{
	NazaraMutex(m_mutex);

	if (m_paused)
	{
		m_refTime = NzGetMicroseconds();
		m_paused = false;
	}
	else
		NazaraWarning("Clock is not paused, ignoring...");
}

nzUInt64 NzGetMicrosecondsLowPrecision()
{
	return NzClockImplGetMilliseconds()*1000ULL;
}

nzUInt64 NzGetMicrosecondsFirstRun()
{
	if (NzClockImplInitializeHighPrecision())
		NzGetMicroseconds = NzClockImplGetMicroseconds;
	else
		NzGetMicroseconds = NzGetMicrosecondsLowPrecision;

	return NzGetMicroseconds();
}

NzClockFunction NzGetMicroseconds = NzGetMicrosecondsFirstRun;
NzClockFunction NzGetMilliseconds = NzClockImplGetMilliseconds;

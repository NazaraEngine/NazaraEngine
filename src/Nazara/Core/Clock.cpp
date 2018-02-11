// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Clock.hpp>

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

	/*!
	* \ingroup core
	* \class Nz::Clock
	* \brief Utility class that measure the elapsed time
	*/

	/*!
	* \brief Constructs a Clock object
	*
	* \param startingValue The starting time value, in microseconds
	* \param paused The clock pause state
	*/
	Clock::Clock(UInt64 startingValue, bool paused) :
	m_elapsedTime(startingValue),
	m_refTime(GetElapsedMicroseconds()),
	m_paused(paused)
	{
	}

	/*!
	* Returns the elapsed time in seconds
	* \return Seconds elapsed
	*
	* \see GetMicroseconds, GetMilliseconds
	*/
	float Clock::GetSeconds() const
	{
		return GetMicroseconds()/1'000'000.f;
	}

	/*!
	* Returns the elapsed time in microseconds
	* \return Microseconds elapsed
	*
	* \see GetMilliseconds, GetSeconds
	*/
	UInt64 Clock::GetMicroseconds() const
	{
		NazaraLock(m_mutex);

		UInt64 elapsedMicroseconds = m_elapsedTime;
		if (!m_paused)
			elapsedMicroseconds += (GetElapsedMicroseconds() - m_refTime);

		return elapsedMicroseconds;
	}

	/*!
	* Returns the elapsed time in milliseconds
	* \return Milliseconds elapsed
	*
	* \see GetMicroseconds, GetSeconds
	*/
	UInt64 Clock::GetMilliseconds() const
	{
		return GetMicroseconds()/1000;
	}

	/*!
	* Returns the current pause state of the clock
	* \return Boolean indicating if the clock is currently paused
	*
	* \see Pause, Unpause
	*/
	bool Clock::IsPaused() const
	{
		NazaraLock(m_mutex);

		return m_paused;
	}

	/*!
	* \brief Pause the clock
	*
	* Pauses the clock, making the time retrieving functions to always return the value at the time the clock was paused
	* This has no effect if the clock is already paused
	*
	* \see IsPaused, Unpause
	*/
	void Clock::Pause()
	{
		NazaraLock(m_mutex);

		if (!m_paused)
		{
			m_elapsedTime += GetElapsedMicroseconds() - m_refTime;
			m_paused = true;
		}
	}

	/*!
	* \brief Restart the clock
	* \return Microseconds elapsed
	*
	* Restarts the clock, putting it's time counter back to zero (as if the clock got constructed).
	* It also compute the elapsed microseconds since the last Restart() call without any time loss (a problem that the combination of GetElapsedMicroseconds and Restart have).
	*/
	UInt64 Clock::Restart()
	{
		NazaraLock(m_mutex);

		Nz::UInt64 now = GetElapsedMicroseconds();

		Nz::UInt64 elapsedTime = m_elapsedTime;
		if (!m_paused)
			elapsedTime += (now - m_refTime);

		m_elapsedTime = 0;
		m_refTime = now;
		m_paused = false;

		return elapsedTime;
	}

	/*!
	* \brief Unpause the clock
	*
	* Unpauses the clock, making the clock continue to measure the time
	* This has no effect if the clock is already unpaused
	*
	* \see IsPaused, Unpause
	*/
	void Clock::Unpause()
	{
		NazaraLock(m_mutex);

		if (m_paused)
		{
			m_refTime = GetElapsedMicroseconds();
			m_paused = false;
		}
	}

	ClockFunction GetElapsedMicroseconds = Detail::GetElapsedMicrosecondsFirstRun;
	ClockFunction GetElapsedMilliseconds = ClockImplGetElapsedMilliseconds;
}

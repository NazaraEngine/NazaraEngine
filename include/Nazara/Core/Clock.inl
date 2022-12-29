// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
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
	template<bool HighPrecision>
	Clock<HighPrecision>::Clock(Time startingValue, bool paused) :
	m_elapsedTime(startingValue),
	m_refTime(Now()),
	m_paused(paused)
	{
	}

	/*!
	* Returns the elapsed time
	* \return Duration elapsed
	*/
	template<bool HighPrecision>
	Time Clock<HighPrecision>::GetElapsedTime() const
	{
		Time elapsedNanoseconds = m_elapsedTime;
		if (!m_paused)
			elapsedNanoseconds += Now() - m_refTime;

		return elapsedNanoseconds;
	}

	/*!
	* Returns the current pause state of the clock
	* \return Boolean indicating if the clock is currently paused
	*
	* \see Pause, Unpause
	*/
	template<bool HighPrecision>
	bool Clock<HighPrecision>::IsPaused() const
	{
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
	template<bool HighPrecision>
	void Clock<HighPrecision>::Pause()
	{
		if (!m_paused)
		{
			m_elapsedTime += Now() - m_refTime;
			m_paused = true;
		}
	}

	/*!
	* \brief Restart the clock
	* \return Time elapsed since creation or last restart call
	*
	* Restarts the clock, putting its time counter back to the starting value
	* It also compute the elapsed microseconds since the last Restart() call without any time loss (a problem that the combination of GetElapsedTime and Restart have).
	*/
	template<bool HighPrecision>
	Time Clock<HighPrecision>::Restart(Time startingValue, bool paused)
	{
		Time now = Now();

		Time elapsedTime = m_elapsedTime;
		if (!m_paused)
			elapsedTime += now - m_refTime;

		m_elapsedTime = startingValue;
		m_refTime = now;
		m_paused = paused;

		return elapsedTime;
	}

	/*!
	* \brief Restart the clock if more than time elapsed
	* \return If more than time elapsed since creation or last restart call
	*
	* Restarts the clock, putting its time counter back to zero
	* This function allows to check the elapsed time of a clock and restart it if over some value in a single call, preventing some loss between GetElapsedTime and Restart
	*/
	template<bool HighPrecision>
	std::optional<Time> Clock<HighPrecision>::RestartIfOver(Time time)
	{
		Time now = Now();

		Time elapsedTime = m_elapsedTime;
		if (!m_paused)
			elapsedTime += now - m_refTime;

		if (elapsedTime < time)
			return std::nullopt;

		m_elapsedTime = Time::Zero();
		m_refTime = now;

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
	template<bool HighPrecision>
	void Clock<HighPrecision>::Unpause()
	{
		if (m_paused)
		{
			m_refTime = Now();
			m_paused = false;
		}
	}

	template<bool HighPrecision>
	Time Clock<HighPrecision>::Now()
	{
		if constexpr (HighPrecision)
			return GetElapsedNanoseconds();
		else
			return GetElapsedMilliseconds();
	}
}

#include <Nazara/Core/DebugOff.hpp>

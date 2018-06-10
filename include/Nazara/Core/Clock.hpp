// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CLOCK_HPP
#define NAZARA_CLOCK_HPP

#include <Nazara/Prerequisites.hpp>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_CLOCK
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

namespace Nz
{
	class NAZARA_CORE_API Clock
	{
		public:
			Clock(UInt64 startingValue = 0, bool paused = false);
			Clock(const Clock& clock) = default;
			Clock(Clock&& clock) = default;
			~Clock() = default;

			float GetSeconds() const;
			UInt64 GetMicroseconds() const;
			UInt64 GetMilliseconds() const;

			bool IsPaused() const;

			void Pause();
			UInt64 Restart();
			void Unpause();

			Clock& operator=(const Clock& clock) = default;
			Clock& operator=(Clock&& clock) = default;

		private:
			NazaraMutexAttrib(m_mutex, mutable)

			UInt64 m_elapsedTime;
			UInt64 m_refTime;
			bool m_paused;
	};

	using ClockFunction = UInt64 (*)();

	extern NAZARA_CORE_API ClockFunction GetElapsedMicroseconds;
	extern NAZARA_CORE_API ClockFunction GetElapsedMilliseconds;
}

#endif // NAZARA_CLOCK_HPP

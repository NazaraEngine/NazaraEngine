// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_CLOCK_HPP
#define NAZARA_CORE_CLOCK_HPP

#include <Nazara/Prerequisites.hpp>

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
			UInt64 m_elapsedTime;
			UInt64 m_refTime;
			bool m_paused;
	};

	using ClockFunction = UInt64 (*)();

	extern NAZARA_CORE_API ClockFunction GetElapsedMicroseconds;
	extern NAZARA_CORE_API ClockFunction GetElapsedMilliseconds;
}

#endif // NAZARA_CORE_CLOCK_HPP

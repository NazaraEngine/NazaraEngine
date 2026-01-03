// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_CLOCK_HPP
#define NAZARA_CORE_CLOCK_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Time.hpp>
#include <optional>

namespace Nz
{
	template<bool HighPrecision>
	class Clock
	{
		public:
			Clock(Time startingValue = Time::Zero(), bool paused = false);
			Clock(const Clock& clock) = default;
			Clock(Clock&& clock) noexcept = default;
			~Clock() = default;

			Time GetElapsedTime() const;

			bool IsPaused() const;

			void Pause();

			Time Restart(Time startingPoint = Time::Zero(), bool paused = false);
			std::optional<Time> RestartIfOver(Time time);

			bool Tick(Time time);

			void Unpause();

			Clock& operator=(const Clock& clock) = default;
			Clock& operator=(Clock&& clock) noexcept = default;

			static Time Now();

		private:
			Time m_elapsedTime;
			Time m_refTime;
			bool m_paused;
	};

	using HighPrecisionClock = Clock<true>;
	using MillisecondClock = Clock<false>;
}

#include <Nazara/Core/Clock.inl>

#endif // NAZARA_CORE_CLOCK_HPP

// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_TIMERMANAGER_HPP
#define NAZARA_CORE_TIMERMANAGER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Time.hpp>
#include <functional>
#include <vector>

namespace Nz
{
	class TimerManager
	{
		public:
			using Callback = std::function<void()>;

			inline TimerManager();
			TimerManager(const TimerManager&) = delete;
			TimerManager(TimerManager&&) = delete;
			~TimerManager() = default;

			inline void AddTimer(Time triggerDelay, Callback callback, bool isRepeating = false);
			inline void AddTimer(Time triggerDelay, Callback callback, bool isRepeating, Time initialDelay = Time::Zero());
			inline void AddImmediateTimer(Callback callback);

			inline void Update(Time elapsedTime);

			TimerManager& operator=(const TimerManager&) = delete;
			TimerManager& operator=(TimerManager&&) = delete;

		private:
			struct Entry
			{
				Callback callback;
				Time triggerTime;
				Time delay;
				bool isRepeating;
			};

			std::vector<Entry> m_timers;
			Time m_currentTime;
	};
}

#include <Nazara/Core/TimerManager.inl>

#endif // NAZARA_CORE_TIMERMANAGER_HPP

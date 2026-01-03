// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <algorithm>

namespace Nz
{
	inline TimerManager::TimerManager() :
	m_currentTime(Time::Zero())
	{
	}

	inline void TimerManager::AddTimer(Time triggerDelay, Callback callback, bool isRepeating)
	{
		return AddTimer(triggerDelay, std::move(callback), isRepeating, triggerDelay);
	}

	inline void TimerManager::AddTimer(Time triggerDelay, Callback callback, bool isRepeating, Time initialDelay)
	{
		Time triggerTime = m_currentTime + initialDelay;
		auto it = std::upper_bound(m_timers.begin(), m_timers.end(), triggerTime, [](Time triggerTime, const Entry& entry) { return entry.triggerTime > triggerTime; });
		m_timers.emplace(it, Entry{
			.callback = std::move(callback),
			.triggerTime = triggerTime,
			.delay = triggerDelay,
			.isRepeating = isRepeating
		});
	}

	inline void TimerManager::AddImmediateTimer(Callback callback)
	{
		auto it = std::upper_bound(m_timers.begin(), m_timers.end(), m_currentTime, [](Time triggerTime, const Entry& entry) { return entry.triggerTime > triggerTime; });
		m_timers.emplace(it, Entry{
			.callback = std::move(callback),
			.triggerTime = m_currentTime,
			.delay = Time::Zero(),
			.isRepeating = false
		});
	}

	inline void TimerManager::Update(Time elapsedTime)
	{
		m_currentTime += elapsedTime;

		auto it = m_timers.begin();
		while (it != m_timers.end())
		{
			Entry& timer = *it;
			if (timer.triggerTime > m_currentTime)
				break;

			timer.callback();
			if (timer.isRepeating)
			{
				Time triggerTime = m_currentTime + timer.delay;
				auto insertionIt = std::lower_bound(it, m_timers.end(), triggerTime, [](const Entry& entry, Time triggerTime) { return entry.triggerTime < triggerTime; });
				if (insertionIt != it)
				{
					*insertionIt = std::move(*it);
					insertionIt->triggerTime = triggerTime;
				}
			}
			++it;
		}
		m_timers.erase(m_timers.begin(), it);
	}
}

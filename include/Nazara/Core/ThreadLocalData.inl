// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	template<typename T>
	ThreadLocalData<T>::~ThreadLocalData()
	{
		for (auto&& [threadId, entry] : m_data)
		{
			NazaraUnused(threadId);
			entry.watcher->Unregister(this);
		}
	}

	template<typename T>
	template<typename... Args>
	inline T& ThreadLocalData<T>::GetOrCreate(Args&&... args)
	{
		std::unique_lock lock(m_mutex);
		std::thread::id threadId = std::this_thread::get_id();
		auto it = m_data.find(threadId);
		if (it == m_data.end())
		{
			it = m_data.emplace(threadId, DataEntry{}).first;
			it->second.data = std::make_unique<T>(std::forward<Args>(args)...);
			it->second.watcher = &Detail::ThreadLocalDataWatcher::GetThreadWatcher();
			it->second.watcher->Register(this);
		}

		return *it->second.data;
	}

	template<typename T>
	void ThreadLocalData<T>::OnThreadExit(std::thread::id threadId)
	{
		std::unique_lock lock(m_mutex);
		m_data.erase(threadId);
	}
}


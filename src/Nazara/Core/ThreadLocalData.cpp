// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/ThreadLocalData.hpp>
#include <NazaraUtils/Assert.hpp>

namespace Nz::Detail
{
	void ThreadLocalDataWatcher::Register(ThreadLocalDataBase* localData)
	{
		std::unique_lock lock(m_mutex);
		NazaraAssert(std::find(m_dataList.begin(), m_dataList.end(), localData) == m_dataList.end());
		m_dataList.push_back(localData);
	}

	void ThreadLocalDataWatcher::Unregister(ThreadLocalDataBase* localData)
	{
		std::unique_lock lock(m_mutex);
		auto it = std::find(m_dataList.begin(), m_dataList.end(), localData);
		NazaraAssert(it != m_dataList.end());
		m_dataList.erase(it);
	}

	ThreadLocalDataWatcher& ThreadLocalDataWatcher::GetThreadWatcher()
	{
		static thread_local ThreadLocalDataWatcher watcher;
		return watcher;
	}

	ThreadLocalDataWatcher::~ThreadLocalDataWatcher()
	{
		std::thread::id threadId = std::this_thread::get_id();
		for (ThreadLocalDataBase* data : m_dataList)
			data->OnThreadExit(threadId);
	}
}

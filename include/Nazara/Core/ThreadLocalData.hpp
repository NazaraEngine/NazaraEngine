// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_THREADLOCALDATA_HPP
#define NAZARA_CORE_THREADLOCALDATA_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <Nazara/Core/Export.hpp>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <thread>
#include <vector>

namespace Nz
{
	template<typename T> class ThreadLocalData;

	namespace Detail
	{
		class NAZARA_CORE_API ThreadLocalDataBase
		{
			friend class ThreadLocalDataWatcher;

			protected:
				virtual void OnThreadExit(std::thread::id threadId) = 0;
		};

		class NAZARA_CORE_API ThreadLocalDataWatcher
		{
			public:
				ThreadLocalDataWatcher() = default;
				ThreadLocalDataWatcher(const ThreadLocalDataWatcher&) = delete;
				ThreadLocalDataWatcher(ThreadLocalDataWatcher&&) = delete;
				~ThreadLocalDataWatcher();

				void Register(ThreadLocalDataBase* localData);
				void Unregister(ThreadLocalDataBase* localData);

				ThreadLocalDataWatcher& operator=(const ThreadLocalDataWatcher&) = delete;
				ThreadLocalDataWatcher& operator=(ThreadLocalDataWatcher&&) = delete;

				static ThreadLocalDataWatcher& GetThreadWatcher();

			private:
				template<typename T> friend class ThreadLocalData;

				std::mutex m_mutex;
				std::vector<ThreadLocalDataBase*> m_dataList;
		};
	}

	template<typename T>
	class ThreadLocalData : Detail::ThreadLocalDataBase
	{
		public:
			ThreadLocalData() = default;
			ThreadLocalData(const ThreadLocalData&) = delete;
			ThreadLocalData(ThreadLocalData&&) = delete;
			~ThreadLocalData();

			template<typename... Args> T& GetOrCreate(Args&&... args);

			ThreadLocalData& operator=(const ThreadLocalData&) = delete;
			ThreadLocalData& operator=(ThreadLocalData&&) = delete;

		private:
			void OnThreadExit(std::thread::id threadId) override;

			struct DataEntry
			{
				MovablePtr<Detail::ThreadLocalDataWatcher> watcher;
				std::unique_ptr<T> data;
			};

			std::mutex m_mutex;
			std::unordered_map<std::thread::id, DataEntry> m_data;
	};
}

#include <Nazara/Core/ThreadLocalData.inl>

#endif // NAZARA_CORE_THREADLOCALDATA_HPP

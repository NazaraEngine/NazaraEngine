// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_TASKSCHEDULER_HPP
#define NAZARA_CORE_TASKSCHEDULER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <atomic>
#include <functional>
#include <memory>

namespace Nz
{
	class NAZARA_CORE_API TaskScheduler
	{
		public:
			using Task = std::function<void()>;

			TaskScheduler(unsigned int workerCount = 0);
			TaskScheduler(const TaskScheduler&) = delete;
			TaskScheduler(TaskScheduler&&) = delete;
			~TaskScheduler();

			void AddTask(Task&& task);

			unsigned int GetWorkerCount() const;

			void WaitForTasks();

			TaskScheduler& operator=(const TaskScheduler&) = delete;
			TaskScheduler& operator=(TaskScheduler&&) = delete;

		private:
			class Worker;
			friend Worker;

			Worker& GetWorker(unsigned int workerIndex);
			void NotifyWorkerActive();
			void NotifyWorkerIdle();

			std::atomic_bool m_idle;
			std::atomic_uint m_idleWorkerCount;
			std::size_t m_nextWorkerIndex;
			std::vector<Worker> m_workers;
	};
}

#include <Nazara/Core/TaskScheduler.inl>

#endif // NAZARA_CORE_TASKSCHEDULER_HPP

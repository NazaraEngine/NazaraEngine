// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_TASKSCHEDULER_HPP
#define NAZARA_CORE_TASKSCHEDULER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
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
			struct Data;
			class Worker;
			std::unique_ptr<Data> m_data;
	};
}

#include <Nazara/Core/TaskScheduler.inl>

#endif // NAZARA_CORE_TASKSCHEDULER_HPP

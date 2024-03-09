// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_TASKSCHEDULERAPPCOMPONENT_HPP
#define NAZARA_CORE_TASKSCHEDULERAPPCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/TaskScheduler.hpp>

namespace Nz
{
	class TaskSchedulerAppComponent final : public ApplicationComponent
	{
		public:
			inline TaskSchedulerAppComponent(ApplicationBase& app, unsigned int workerCount = 0);
			TaskSchedulerAppComponent(const TaskSchedulerAppComponent&) = delete;
			TaskSchedulerAppComponent(TaskSchedulerAppComponent&&) = delete;
			~TaskSchedulerAppComponent() = default;

			inline void AddTask(TaskScheduler::Task&& task);

			inline TaskScheduler& GetScheduler();
			inline const TaskScheduler& GetScheduler() const;
			inline unsigned int GetWorkerCount() const;

			inline void WaitForTasks();

			TaskSchedulerAppComponent& operator=(const TaskSchedulerAppComponent&) = delete;
			TaskSchedulerAppComponent& operator=(TaskSchedulerAppComponent&&) = delete;

		private:
			TaskScheduler m_scheduler;
	};
}

#include <Nazara/Core/TaskSchedulerAppComponent.inl>

#endif // NAZARA_CORE_TASKSCHEDULERAPPCOMPONENT_HPP

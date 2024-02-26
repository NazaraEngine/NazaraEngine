// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline TaskSchedulerAppComponent::TaskSchedulerAppComponent(ApplicationBase& app, unsigned int workerCount) :
	ApplicationComponent(app),
	m_scheduler(workerCount)
	{
	}

	inline void TaskSchedulerAppComponent::AddTask(TaskScheduler::Task&& task)
	{
		return m_scheduler.AddTask(std::move(task));
	}

	inline TaskScheduler& TaskSchedulerAppComponent::GetScheduler()
	{
		return m_scheduler;
	}

	inline const TaskScheduler& TaskSchedulerAppComponent::GetScheduler() const
	{
		return m_scheduler;
	}

	inline unsigned int TaskSchedulerAppComponent::GetWorkerCount() const
	{
		return m_scheduler.GetWorkerCount();
	}

	inline void TaskSchedulerAppComponent::WaitForTasks()
	{
		return m_scheduler.WaitForTasks();
	}
}

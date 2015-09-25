// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/HardwareInfo.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/TaskSchedulerImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/TaskSchedulerImpl.hpp>
#else
	#error Lack of implementation: Task Scheduler
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
		std::vector<Functor*> s_pendingWorks;
		unsigned int s_workerCount = 0;
	}

	unsigned int TaskScheduler::GetWorkerCount()
	{
		return (s_workerCount > 0) ? s_workerCount : HardwareInfo::GetProcessorCount();
	}

	bool TaskScheduler::Initialize()
	{
		return TaskSchedulerImpl::Initialize(GetWorkerCount());
	}

	void TaskScheduler::Run()
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize Task Scheduler");
			return;
		}

		if (!s_pendingWorks.empty())
		{
			TaskSchedulerImpl::Run(&s_pendingWorks[0], s_pendingWorks.size());
			s_pendingWorks.clear();
		}
	}

	void TaskScheduler::SetWorkerCount(unsigned int workerCount)
	{
		#ifdef NAZARA_CORE_SAFE
		if (TaskSchedulerImpl::IsInitialized())
		{
			NazaraError("Worker count cannot be set while initialized");
			return;
		}
		#endif

		s_workerCount = workerCount;
	}

	void TaskScheduler::Uninitialize()
	{
		if (TaskSchedulerImpl::IsInitialized())
			TaskSchedulerImpl::Uninitialize();
	}

	void TaskScheduler::WaitForTasks()
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize Task Scheduler");
			return;
		}

		TaskSchedulerImpl::WaitForTasks();
	}

	void TaskScheduler::AddTaskFunctor(Functor* taskFunctor)
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize Task Scheduler");
			return;
		}

		s_pendingWorks.push_back(taskFunctor);
	}
}

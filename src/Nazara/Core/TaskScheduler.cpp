// Copyright (C) 2013 Jérôme Leclercq
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

namespace
{
	std::vector<NzFunctor*> s_pendingWorks;
	unsigned int s_workerCount = 0;
}

unsigned int NzTaskScheduler::GetWorkerCount()
{
	return (s_workerCount > 0) ? s_workerCount : NzHardwareInfo::GetProcessorCount();
}

bool NzTaskScheduler::Initialize()
{
	return NzTaskSchedulerImpl::Initialize(GetWorkerCount());
}

void NzTaskScheduler::Run()
{
	NzTaskSchedulerImpl::Run(&s_pendingWorks[0], s_pendingWorks.size());
	s_pendingWorks.clear();
}

void NzTaskScheduler::SetWorkerCount(unsigned int workerCount)
{
	#ifdef NAZARA_CORE_SAFE
	if (NzTaskSchedulerImpl::IsInitialized())
	{
		NazaraError("Worker count cannot be set while initialized");
		return;
	}
	#endif

	s_workerCount = workerCount;
}

void NzTaskScheduler::Uninitialize()
{
	if (NzTaskSchedulerImpl::IsInitialized())
		NzTaskSchedulerImpl::Uninitialize();
}

void NzTaskScheduler::WaitForTasks()
{
	NzTaskSchedulerImpl::WaitForTasks();
}

void NzTaskScheduler::AddTaskFunctor(NzFunctor* taskFunctor)
{
	#ifdef NAZARA_CORE_SAFE
	if (!NzTaskSchedulerImpl::IsInitialized())
	{
		NazaraError("Task scheduler is not initialized");
		return;
	}
	#endif

	s_pendingWorks.push_back(taskFunctor);
}

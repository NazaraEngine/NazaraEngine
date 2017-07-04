// Copyright (C) 2017 Jérôme Leclercq
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

	/*!
	* \ingroup core
	* \class Nz::TaskScheduler
	* \brief Core class that represents a pool of threads
	*
	* \remark Initialized should be called first
	*/

	/*!
	* \brief Gets the number of threads
	* \return Number of threads, if none, the number of simulatenous threads on the processor is returned
	*/

	unsigned int TaskScheduler::GetWorkerCount()
	{
		return (s_workerCount > 0) ? s_workerCount : HardwareInfo::GetProcessorCount();
	}

	/*!
	* \brief Initializes the TaskScheduler class
	* \return true if everything is ok
	*/

	bool TaskScheduler::Initialize()
	{
		return TaskSchedulerImpl::Initialize(GetWorkerCount());
	}

	/*!
	* \brief Runs the pending works
	*
	* \remark Produce a NazaraError if the class is not initialized
	*/

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

	/*!
	* \brief Sets the number of workers
	*
	* \param workerCount Number of simulatnous threads handling the tasks
	*
	* \remark Produce a NazaraError if the class is not initialized and NAZARA_CORE_SAFE is defined
	*/

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

	/*!
	* \brief Uninitializes the TaskScheduler class
	*/

	void TaskScheduler::Uninitialize()
	{
		if (TaskSchedulerImpl::IsInitialized())
			TaskSchedulerImpl::Uninitialize();
	}

	/*!
	* \brief Waits for tasks to be done
	*
	* \remark Produce a NazaraError if the class is not initialized
	*/

	void TaskScheduler::WaitForTasks()
	{
		if (!Initialize())
		{
			NazaraError("Failed to initialize Task Scheduler");
			return;
		}

		TaskSchedulerImpl::WaitForTasks();
	}

	/*!
	* \brief Adds a task on the pending list
	*
	* \param taskFunctor Functor represeting a task to be done
	*
	* \remark Produce a NazaraError if the class is not initialized
	* \remark A task containing a call on this class is undefined behaviour
	*/

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

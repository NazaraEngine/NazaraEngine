// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Thread.hpp>
#include <queue>
#include <vector>
#include <Nazara/Core/Debug.hpp>

namespace
{
	struct TaskSchedulerImpl
	{
		std::queue<NzFunctor*> tasks;
		std::vector<NzThread*> workers;
		NzConditionVariable waiterConditionVariable;
		NzConditionVariable workerConditionVariable;
		NzMutex taskMutex;
		NzMutex waiterConditionVariableMutex;
		NzMutex workerConditionVariableMutex;
		volatile bool running = true;
	};

	TaskSchedulerImpl* s_impl = nullptr;

	void WorkerFunc()
	{
		do
		{
			NzFunctor* task;
			{
				NzLockGuard lock(s_impl->taskMutex);
				if (!s_impl->tasks.empty())
				{
					task = s_impl->tasks.front();
					s_impl->tasks.pop();
				}
				else
					task = nullptr;
			}

			// Avons-nous une tâche ?
			if (task)
				task->Run(); // Chouette ! Allons travailler gaiement
			else
			{
				// On peut signaler à tout le monde qu'il n'y a plus de tâches
				s_impl->waiterConditionVariableMutex.Lock();
				s_impl->waiterConditionVariable.SignalAll();
				s_impl->waiterConditionVariableMutex.Unlock();

				// Nous attendons qu'une nouvelle tâche arrive
				s_impl->workerConditionVariableMutex.Lock();
				s_impl->workerConditionVariable.Wait(&s_impl->workerConditionVariableMutex);
				s_impl->workerConditionVariableMutex.Unlock();
			}
		}
		while (s_impl->running);
	}
}

unsigned int NzTaskScheduler::GetWorkerCount()
{
	#ifdef NAZARA_CORE_SAFE
	if (!s_impl)
	{
		NazaraError("Task scheduler is not initialized");
		return 0;
	}
	#endif

	return s_impl->workers.size();
}

bool NzTaskScheduler::Initialize()
{
	if (s_impl)
		return true; // Déjà initialisé

	s_impl = new TaskSchedulerImpl;

	unsigned int workerCount = NzThread::HardwareConcurrency();
	for (unsigned int i = 0; i < workerCount; ++i)
	{
		NzThread* thread = new NzThread(WorkerFunc);
		s_impl->workers.push_back(thread);
	}

	return true;
}

void NzTaskScheduler::Uninitialize()
{
	if (s_impl)
	{
		s_impl->running = false;

		s_impl->workerConditionVariableMutex.Lock();
		s_impl->workerConditionVariable.SignalAll();
		s_impl->workerConditionVariableMutex.Unlock();

		for (NzThread* thread : s_impl->workers)
		{
			thread->Join();
			delete thread;
		}

		delete s_impl;
		s_impl = nullptr;
	}
}

void NzTaskScheduler::WaitForTasks()
{
	#ifdef NAZARA_CORE_SAFE
	if (!s_impl)
	{
		NazaraError("Task scheduler is not initialized");
		return;
	}
	#endif

	s_impl->taskMutex.Lock();
	// Tout d'abord, il y a-t-il des tâches en attente ?
	if (s_impl->tasks.empty())
	{
		s_impl->taskMutex.Unlock();
		return;
	}

	// On verrouille d'abord la mutex entourant le signal (Pour ne pas perdre le signal en chemin)
	s_impl->waiterConditionVariableMutex.Lock();
	// Et ensuite seulement on déverrouille la mutex des tâches
	s_impl->taskMutex.Unlock();
	s_impl->waiterConditionVariable.Wait(&s_impl->waiterConditionVariableMutex);
	s_impl->waiterConditionVariableMutex.Unlock();
}

void NzTaskScheduler::AddTaskFunctor(NzFunctor* taskFunctor)
{
	#ifdef NAZARA_CORE_SAFE
	if (!s_impl)
	{
		NazaraError("Task scheduler is not initialized");
		return;
	}
	#endif

	{
		NzLockGuard lock(s_impl->taskMutex);
		s_impl->tasks.push(taskFunctor);
	}

	s_impl->workerConditionVariableMutex.Lock();
	s_impl->workerConditionVariable.Signal();
	s_impl->workerConditionVariableMutex.Unlock();
}

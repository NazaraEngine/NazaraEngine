// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/TaskSchedulerImpl.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstdlib> // std::ldiv
#include <process.h>
#include <Nazara/Core/Debug.hpp>

bool NzTaskSchedulerImpl::Initialize(unsigned int workerCount)
{
	if (s_workerCount > 0)
		return true; // Déjà initialisé

	#if NAZARA_CORE_SAFE
	if (workerCount == 0)
	{
		NazaraError("Invalid worker count ! (0)");
		return false;
	}
	#endif

	s_workerCount = workerCount;
	s_doneEvents.reset(new HANDLE[workerCount]);
	s_workers.reset(new Worker[workerCount]);
	s_workerThreads.reset(new HANDLE[workerCount]);

	// L'identifiant de chaque worker doit rester en vie jusqu'à ce que chaque thread soit correctement lancé
	std::unique_ptr<unsigned int[]> workerIDs(new unsigned int[workerCount]);

	for (unsigned int i = 0; i < workerCount; ++i)
	{
		Worker& worker = s_workers[i];
		InitializeCriticalSection(&worker.queueMutex);
		worker.wakeEvent = CreateEventA(nullptr, false, false, nullptr);
		worker.running = true;
		worker.workCount = 0;

		s_doneEvents[i] = CreateEventA(nullptr, true, false, nullptr);

		workerIDs[i] = i;
		s_workerThreads[i] = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &WorkerProc, &workerIDs[i], 0, nullptr));
	}

	// On attend que les workers se mettent en attente
	WaitForMultipleObjects(s_workerCount, &s_doneEvents[0], true, INFINITE);

	return true;
}

bool NzTaskSchedulerImpl::IsInitialized()
{
	return s_workerCount > 0;
}

void NzTaskSchedulerImpl::Run(NzFunctor** tasks, unsigned int count)
{
	WaitForMultipleObjects(s_workerCount, &s_doneEvents[0], true, INFINITE);

	std::ldiv_t div = std::ldiv(count, s_workerCount); // Division et modulo en une opération, y'a pas de petit profit
	for (unsigned int i = 0; i < s_workerCount; ++i)
	{
		Worker& worker = s_workers[i];
		unsigned int taskCount = (i == 0) ? div.quot + div.rem : div.quot;
		for (unsigned int j = 0; j < taskCount; ++j)
			worker.queue.push(*tasks++);

		worker.workCount = taskCount;
	}

	// On les lance une fois qu'ils sont tous initialisés (pour éviter qu'un worker ne passe en pause détectant une absence de travaux)
	for (unsigned int i = 0; i < s_workerCount; ++i)
	{
		ResetEvent(s_doneEvents[i]);
		SetEvent(s_workers[i].wakeEvent);
	}
}

void NzTaskSchedulerImpl::Uninitialize()
{
	#ifdef NAZARA_CORE_SAFE
	if (s_workerCount == 0)
	{
		NazaraError("Task scheduler is not initialized");
		return;
	}
	#endif

	for (unsigned int i = 0; i < s_workerCount; ++i)
	{
		Worker& worker = s_workers[i];
		worker.running = false;
		worker.workCount = 0;

		EnterCriticalSection(&worker.queueMutex);

		std::queue<NzFunctor*> emptyQueue;
		std::swap(worker.queue, emptyQueue); // Et on vide la queue

		LeaveCriticalSection(&worker.queueMutex);

		SetEvent(worker.wakeEvent);
	}

	WaitForMultipleObjects(s_workerCount, &s_workerThreads[0], true, INFINITE);

	for (unsigned int i = 0; i < s_workerCount; ++i)
	{
		Worker& worker = s_workers[i];
		CloseHandle(s_doneEvents[i]);
		CloseHandle(s_workerThreads[i]);
		CloseHandle(worker.wakeEvent);
		DeleteCriticalSection(&worker.queueMutex);
	}

	s_doneEvents.reset();
	s_workers.reset();
	s_workerThreads.reset();
	s_workerCount = 0;
}

void NzTaskSchedulerImpl::WaitForTasks()
{
	#ifdef NAZARA_CORE_SAFE
	if (s_workerCount == 0)
	{
		NazaraError("Task scheduler is not initialized");
		return;
	}
	#endif

	WaitForMultipleObjects(s_workerCount, &s_doneEvents[0], true, INFINITE);
}

NzFunctor* NzTaskSchedulerImpl::StealTask(unsigned int workerID)
{
	bool shouldRetry;
	do
	{
		shouldRetry = false;
		for (unsigned int i = 0; i < s_workerCount; ++i)
		{
			if (i == workerID)
				continue;

			Worker& worker = s_workers[i];

			if (worker.workCount > 0)
			{
				NzFunctor* task;
				if (TryEnterCriticalSection(&worker.queueMutex))
				{
					if (!worker.queue.empty())
					{
						task = worker.queue.front();
						worker.queue.pop();
						worker.workCount = worker.queue.size();
					}
					else
						task = nullptr;

					LeaveCriticalSection(&worker.queueMutex);
				}
				else
					shouldRetry = true; // Il est encore possible d'avoir un job

				if (task)
					return task;
			}
		}
	}
	while (shouldRetry);

	return nullptr;
}

unsigned int __stdcall NzTaskSchedulerImpl::WorkerProc(void* userdata)
{
	unsigned int workerID = *reinterpret_cast<unsigned int*>(userdata);
	SetEvent(s_doneEvents[workerID]);

	Worker& worker = s_workers[workerID];
	WaitForSingleObject(worker.wakeEvent, INFINITE);

	while (worker.running)
	{
		NzFunctor* task = nullptr;

		if (worker.workCount > 0) // Permet d'éviter d'entrer inutilement dans une section critique
		{
			EnterCriticalSection(&worker.queueMutex);
			if (!worker.queue.empty()) // Nécessaire car le workCount peut être tombé à zéro juste avant l'entrée dans la section critique
			{
				task = worker.queue.front();
				worker.queue.pop();
				worker.workCount = worker.queue.size();
			}
			LeaveCriticalSection(&worker.queueMutex);
		}

		if (!task)
			task = StealTask(workerID);

		if (task)
		{
			task->Run();
			delete task;
		}
		else
		{
			SetEvent(s_doneEvents[workerID]);
			WaitForSingleObject(worker.wakeEvent, INFINITE);
		}
	}

	SetEvent(s_doneEvents[workerID]); // Au cas où un thread attendrait sur WaitForTasks() pendant qu'un autre appellerait Uninitialize()

	return 0;
}

std::unique_ptr<HANDLE[]> NzTaskSchedulerImpl::s_doneEvents; // Doivent être contigus
std::unique_ptr<NzTaskSchedulerImpl::Worker[]> NzTaskSchedulerImpl::s_workers;
std::unique_ptr<HANDLE[]> NzTaskSchedulerImpl::s_workerThreads; // Doivent être contigus
unsigned int NzTaskSchedulerImpl::s_workerCount;

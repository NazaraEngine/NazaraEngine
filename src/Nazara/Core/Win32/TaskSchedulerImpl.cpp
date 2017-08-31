// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/TaskSchedulerImpl.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstdlib> // std::ldiv
#include <process.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	bool TaskSchedulerImpl::Initialize(std::size_t workerCount)
	{
		if (IsInitialized())
			return true; // Déjà initialisé

		#if NAZARA_CORE_SAFE
		if (workerCount == 0)
		{
			NazaraError("Invalid worker count ! (0)");
			return false;
		}
		#endif

		s_workerCount = static_cast<DWORD>(workerCount);
		s_doneEvents.reset(new HANDLE[workerCount]);
		s_workers.reset(new Worker[workerCount]);
		s_workerThreads.reset(new HANDLE[workerCount]);

		// L'identifiant de chaque worker doit rester en vie jusqu'à ce que chaque thread soit correctement lancé
		std::unique_ptr<std::size_t[]> workerIDs(new std::size_t[workerCount]);

		for (std::size_t i = 0; i < workerCount; ++i)
		{
			// On initialise les évènements, mutex et threads de chaque worker
			Worker& worker = s_workers[i];
			InitializeCriticalSection(&worker.queueMutex);
			worker.wakeEvent = CreateEventW(nullptr, false, false, nullptr);
			worker.running = true;
			worker.workCount = 0;

			s_doneEvents[i] = CreateEventW(nullptr, true, false, nullptr);

			// Le thread va se lancer, signaler qu'il est prêt à travailler (s_doneEvents) et attendre d'être réveillé
			workerIDs[i] = i;
			s_workerThreads[i] = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &WorkerProc, &workerIDs[i], 0, nullptr));
		}

		// On attend que les workers se mettent en attente
		WaitForMultipleObjects(s_workerCount, &s_doneEvents[0], true, INFINITE);

		return true;
	}

	bool TaskSchedulerImpl::IsInitialized()
	{
		return s_workerCount > 0;
	}

	void TaskSchedulerImpl::Run(Functor** tasks, std::size_t count)
	{
		// On s'assure que des tâches ne sont pas déjà en cours
		WaitForMultipleObjects(s_workerCount, &s_doneEvents[0], true, INFINITE);

		std::lldiv_t div = std::lldiv(count, s_workerCount); // Division et modulo en une opération, y'a pas de petit profit
		for (std::size_t i = 0; i < s_workerCount; ++i)
		{
			// On va maintenant répartir les tâches entre chaque worker et les envoyer dans la queue de chacun
			Worker& worker = s_workers[i];
			std::size_t taskCount = (i == 0) ? div.quot + div.rem : div.quot;
			for (std::size_t j = 0; j < taskCount; ++j)
				worker.queue.push(*tasks++);

			// On stocke le nombre de tâches à côté dans un entier atomique pour éviter d'entrer inutilement dans une section critique
			worker.workCount = taskCount;
		}

		// On les lance une fois qu'ils sont tous initialisés (pour éviter qu'un worker ne passe en pause détectant une absence de travaux)
		for (std::size_t i = 0; i < s_workerCount; ++i)
		{
			ResetEvent(s_doneEvents[i]);
			SetEvent(s_workers[i].wakeEvent);
		}
	}

	void TaskSchedulerImpl::Uninitialize()
	{
		#ifdef NAZARA_CORE_SAFE
		if (s_workerCount == 0)
		{
			NazaraError("Task scheduler is not initialized");
			return;
		}
		#endif

		// On commence par vider la queue de chaque worker pour s'assurer qu'ils s'arrêtent
		for (unsigned int i = 0; i < s_workerCount; ++i)
		{
			Worker& worker = s_workers[i];
			worker.running = false;
			worker.workCount = 0;

			EnterCriticalSection(&worker.queueMutex);

			std::queue<Functor*> emptyQueue;
			std::swap(worker.queue, emptyQueue); // Et on vide la queue (merci std::swap)

			LeaveCriticalSection(&worker.queueMutex);

			// On réveille le worker pour qu'il sorte de la boucle et termine le thread
			SetEvent(worker.wakeEvent);
		}

		// On attend que chaque thread se termine
		WaitForMultipleObjects(s_workerCount, &s_workerThreads[0], true, INFINITE);

		// Et on libère les ressources
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

	void TaskSchedulerImpl::WaitForTasks()
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

	Functor* TaskSchedulerImpl::StealTask(std::size_t workerID)
	{
		bool shouldRetry;
		do
		{
			shouldRetry = false;
			for (std::size_t i = 0; i < s_workerCount; ++i)
			{
				// On ne vole pas la famille, ni soi-même.
				if (i == workerID)
					continue;

				Worker& worker = s_workers[i];

				// Ce worker a-t-il encore des tâches dans sa file d'attente ?
				if (worker.workCount > 0)
				{
					Functor* task = nullptr;

					// Est-ce qu'il utilise la queue maintenant ?
					if (TryEnterCriticalSection(&worker.queueMutex))
					{
						// Non, super ! Profitons-en pour essayer de lui voler un job
						if (!worker.queue.empty()) // On vérifie que la queue n'est pas vide (peut avoir changé avant le verrouillage)
						{
							// Et hop, on vole la tâche
							task = worker.queue.front();
							worker.queue.pop();
							worker.workCount = worker.queue.size();
						}

						LeaveCriticalSection(&worker.queueMutex);
					}
					else
						shouldRetry = true; // Il est encore possible d'avoir un job

					// Avons-nous notre tâche ?
					if (task)
						return task; // Parfait, sortons de là !
				}
			}
		}
		while (shouldRetry);

		// Bon à priori plus aucun worker n'a de tâche
		return nullptr;
	}

	unsigned int __stdcall TaskSchedulerImpl::WorkerProc(void* userdata)
	{
		unsigned int workerID = *static_cast<unsigned int*>(userdata);
		SetEvent(s_doneEvents[workerID]);

		Worker& worker = s_workers[workerID];
		WaitForSingleObject(worker.wakeEvent, INFINITE);

		while (worker.running)
		{
			Functor* task = nullptr;

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

			// Que faire quand vous n'avez plus de travail ?
			if (!task)
				task = StealTask(workerID); // Voler le travail des autres !

			if (task)
			{
				// On exécute la tâche avant de la supprimer
				task->Run();
				delete task;
			}
			else
			{
				SetEvent(s_doneEvents[workerID]);
				WaitForSingleObject(worker.wakeEvent, INFINITE);
			}
		}

		// Au cas où un thread attendrait sur WaitForTasks() pendant qu'un autre appellerait Uninitialize()
		// Ça ne devrait pas arriver, mais comme ça ne coûte pas grand chose..
		SetEvent(s_doneEvents[workerID]);

		return 0;
	}

	std::unique_ptr<HANDLE[]> TaskSchedulerImpl::s_doneEvents; // Doivent être contigus
	std::unique_ptr<TaskSchedulerImpl::Worker[]> TaskSchedulerImpl::s_workers;
	std::unique_ptr<HANDLE[]> TaskSchedulerImpl::s_workerThreads; // Doivent être contigus
	DWORD TaskSchedulerImpl::s_workerCount;
}

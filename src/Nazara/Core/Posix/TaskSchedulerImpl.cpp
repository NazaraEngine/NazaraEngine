// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/TaskSchedulerImpl.hpp>
#include <Nazara/Core/Functor.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	bool TaskSchedulerImpl::Initialize(unsigned int workerCount)
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

		s_workerCount = workerCount;
		s_isDone = false;
		s_isWaiting = false;
		s_shouldFinish = false;

		s_threads.reset(new pthread_t[workerCount]);

		// On initialise les conditions variables, mutex et barrière.
		pthread_cond_init(&s_cvEmpty, nullptr);
		pthread_cond_init(&s_cvNotEmpty, nullptr);
		pthread_mutex_init(&s_mutexQueue, nullptr);
		pthread_barrier_init(&s_barrier, nullptr, workerCount + 1);

		for (unsigned int i = 0; i < s_workerCount; ++i)
		{
			// Le thread va se lancer, attendre que tous se créent et attendre d'être réveillé.
			pthread_create(&s_threads[i], nullptr, WorkerProc, nullptr);
		}

		pthread_barrier_wait(&s_barrier); // On attend que les enfants soient bien créés.

		return true;
	}

	bool TaskSchedulerImpl::IsInitialized()
	{
		return s_workerCount > 0;
	}

	void TaskSchedulerImpl::Run(Functor** tasks, unsigned int count)
	{
		// On s'assure que des tâches ne sont pas déjà en cours
		Wait();

		pthread_mutex_lock(&s_mutexQueue);
		s_isDone = false;

		while (count--)
			s_tasks.push(*tasks++);

		pthread_cond_signal(&s_cvNotEmpty);
		pthread_mutex_unlock(&s_mutexQueue);
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

		// On réveille les threads pour qu'ils sortent de la boucle et terminent.
		pthread_mutex_lock(&s_mutexQueue);
		// On commence par vider la queue et demander qu'ils s'arrêtent.
		std::queue<Functor*> emptyQueue;
		std::swap(s_tasks, emptyQueue);
		s_shouldFinish = true;
		pthread_cond_broadcast(&s_cvNotEmpty);
		pthread_mutex_unlock(&s_mutexQueue);

		// On attend que chaque thread se termine
		for (unsigned int i = 0; i < s_workerCount; ++i)
			pthread_join(s_threads[i], nullptr);

		// Et on libère les ressources
		pthread_barrier_destroy(&s_barrier);
		pthread_cond_destroy(&s_cvEmpty);
		pthread_cond_destroy(&s_cvNotEmpty);
		pthread_mutex_destroy(&s_mutexQueue);

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

		Wait();
	}

	Functor* TaskSchedulerImpl::PopQueue()
	{
		Functor* task = nullptr;

		pthread_mutex_lock(&s_mutexQueue);

		if (!s_tasks.empty())
		{
			task = s_tasks.front();
			s_tasks.pop();
		}

		pthread_mutex_unlock(&s_mutexQueue);

		return task;
	}

	void TaskSchedulerImpl::Wait()
	{
		if (s_isDone)
			return;

		pthread_mutex_lock(&s_mutexQueue);
		s_isWaiting = true;
		pthread_cond_broadcast(&s_cvNotEmpty);
		pthread_cond_wait(&s_cvEmpty, &s_mutexQueue);
		pthread_mutex_unlock(&s_mutexQueue);

		s_isDone = true;
	}

	void* TaskSchedulerImpl::WorkerProc(void* /*userdata*/)
	{
		// On s'assure que tous les threads soient correctement lancés.
		pthread_barrier_wait(&s_barrier);

		// On quitte s'il doit terminer.
		while (!s_shouldFinish)
		{
			Functor* task = PopQueue();

			if (task)
			{
				// On exécute la tâche avant de la supprimer
				task->Run();
				delete task;
			}
			else
			{
				pthread_mutex_lock(&s_mutexQueue);
				if (s_tasks.empty())
					s_isDone = true;

				while (!(!s_tasks.empty() || s_isWaiting || s_shouldFinish))
					pthread_cond_wait(&s_cvNotEmpty, &s_mutexQueue);

				if (s_tasks.empty() && s_isWaiting)
				{
					// On prévient le thread qui attend que les tâches soient effectuées.
					s_isWaiting = false;
					pthread_cond_signal(&s_cvEmpty);
				}

				pthread_mutex_unlock(&s_mutexQueue);
			}
		}

		return nullptr;
	}

	std::queue<Functor*> TaskSchedulerImpl::s_tasks;
	std::unique_ptr<pthread_t[]> TaskSchedulerImpl::s_threads;
	std::atomic<bool> TaskSchedulerImpl::s_isDone;
	std::atomic<bool> TaskSchedulerImpl::s_isWaiting;
	std::atomic<bool> TaskSchedulerImpl::s_shouldFinish;
	unsigned int TaskSchedulerImpl::s_workerCount;

	pthread_mutex_t TaskSchedulerImpl::s_mutexQueue;
	pthread_cond_t TaskSchedulerImpl::s_cvEmpty;
	pthread_cond_t TaskSchedulerImpl::s_cvNotEmpty;
	pthread_barrier_t TaskSchedulerImpl::s_barrier;
}

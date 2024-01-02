// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_POSIX_TASKSCHEDULERIMPL_HPP
#define NAZARA_CORE_POSIX_TASKSCHEDULERIMPL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <atomic>
#include <memory>
#include <queue>
#include <pthread.h>

#if defined(NAZARA_PLATFORM_MACOS)
	typedef int pthread_barrierattr_t;
	typedef struct
	{
		pthread_mutex_t mutex;
		pthread_cond_t cond;
		int count;
		int tripCount;
	} pthread_barrier_t;
#endif

namespace Nz
{
	struct AbstractFunctor;

	class TaskSchedulerImpl
	{
		public:
			TaskSchedulerImpl() = delete;
			~TaskSchedulerImpl() = delete;

			static bool Initialize(unsigned int workerCount);
			static bool IsInitialized();
			static void Run(AbstractFunctor** tasks, unsigned int count);
			static void Uninitialize();
			static void WaitForTasks();

		private:
			static AbstractFunctor* PopQueue();
			static void Wait();
			static void* WorkerProc(void* userdata);

			static std::queue<AbstractFunctor*> s_tasks;
			static std::unique_ptr<pthread_t[]> s_threads;
			static std::atomic<bool> s_isDone;
			static std::atomic<bool> s_isWaiting;
			static std::atomic<bool> s_shouldFinish;
			static unsigned int s_workerCount;

			static pthread_mutex_t s_mutexQueue;
			static pthread_cond_t s_cvEmpty;
			static pthread_cond_t s_cvNotEmpty;
			static pthread_barrier_t s_barrier;

#if defined(NAZARA_PLATFORM_MACOS)
			static int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);
			static int pthread_barrier_destroy(pthread_barrier_t *barrier);
			static int pthread_barrier_wait(pthread_barrier_t *barrier);
#endif
	};
}

#endif // NAZARA_CORE_POSIX_TASKSCHEDULERIMPL_HPP

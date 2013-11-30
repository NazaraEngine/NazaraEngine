// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TASKSCHEDULERIMPL_HPP
#define NAZARA_TASKSCHEDULERIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Functor.hpp>
#include <atomic>
#include <memory>
#include <queue>
#include <windows.h>

class NzTaskSchedulerImpl
{
	public:
		NzTaskSchedulerImpl() = delete;
		~NzTaskSchedulerImpl() = delete;

		static bool Initialize(unsigned int workerCount);
		static bool IsInitialized();
		static void Run(NzFunctor** tasks, unsigned int count);
		static void Uninitialize();
		static void WaitForTasks();

	private:
		static NzFunctor* StealTask(unsigned int workerID);
		static unsigned int __stdcall WorkerProc(void* userdata);

		struct Worker
		{
			std::atomic_uint workCount;
			std::queue<NzFunctor*> queue;
			CRITICAL_SECTION queueMutex;
			HANDLE wakeEvent;
			volatile bool running;
		};

		static std::unique_ptr<HANDLE[]> s_doneEvents; // Doivent être contigus
		static std::unique_ptr<Worker[]> s_workers;
		static std::unique_ptr<HANDLE[]> s_workerThreads; // Doivent être contigus
		static unsigned int s_workerCount;
};

#endif // NAZARA_TASKSCHEDULERIMPL_HPP

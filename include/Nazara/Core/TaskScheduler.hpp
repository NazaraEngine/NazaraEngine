// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TASKSCHEDULER_HPP
#define NAZARA_TASKSCHEDULER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Functor.hpp>

class NAZARA_API NzTaskScheduler
{
	public:
		NzTaskScheduler() = delete;
		~NzTaskScheduler() = delete;

		template<typename F> static void AddTask(F function);
		template<typename F, typename... Args> static void AddTask(F function, Args... args);
		template<typename C> static void AddTask(void (C::*function)(), C* object);
		static unsigned int GetWorkerCount();
		static bool Initialize();
		static void Run();
		static void SetWorkerCount(unsigned int workerCount);
		static void Uninitialize();
		static void WaitForTasks();

	private:
		static void AddTaskFunctor(NzFunctor* taskFunctor);
};

#include <Nazara/Core/TaskScheduler.inl>

#endif // NAZARA_TASKSCHEDULER_HPP

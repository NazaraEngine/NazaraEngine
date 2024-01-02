// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_TASKSCHEDULER_HPP
#define NAZARA_CORE_TASKSCHEDULER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Functor.hpp>

namespace Nz
{
	class NAZARA_CORE_API TaskScheduler
	{
		public:
			TaskScheduler() = delete;
			~TaskScheduler() = delete;

			template<typename F> static void AddTask(F function);
			template<typename F, typename... Args> static void AddTask(F function, Args&&... args);
			template<typename C> static void AddTask(void (C::*function)(), C* object);
			static unsigned int GetWorkerCount();
			static bool Initialize();
			static void Run();
			static void SetWorkerCount(unsigned int workerCount);
			static void Uninitialize();
			static void WaitForTasks();

		private:
			static void AddTaskFunctor(AbstractFunctor* taskFunctor);
	};
}

#include <Nazara/Core/TaskScheduler.inl>

#endif // NAZARA_CORE_TASKSCHEDULER_HPP

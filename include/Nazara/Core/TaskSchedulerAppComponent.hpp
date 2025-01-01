// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_TASKSCHEDULERAPPCOMPONENT_HPP
#define NAZARA_CORE_TASKSCHEDULERAPPCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <Nazara/Core/TaskScheduler.hpp>

namespace Nz
{
	class TaskSchedulerAppComponent final : public ApplicationComponent, public TaskScheduler
	{
		public:
			inline TaskSchedulerAppComponent(ApplicationBase& app, unsigned int workerCount = 0);
			TaskSchedulerAppComponent(const TaskSchedulerAppComponent&) = delete;
			TaskSchedulerAppComponent(TaskSchedulerAppComponent&&) = delete;
			~TaskSchedulerAppComponent() = default;

			TaskSchedulerAppComponent& operator=(const TaskSchedulerAppComponent&) = delete;
			TaskSchedulerAppComponent& operator=(TaskSchedulerAppComponent&&) = delete;
	};
}

#include <Nazara/Core/TaskSchedulerAppComponent.inl>

#endif // NAZARA_CORE_TASKSCHEDULERAPPCOMPONENT_HPP

// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::TaskScheduler
	* \brief Core class that represents a thread pool
	*/

	/*!
	* \brief Adds a task to the pending list
	*
	* \param function Task that the pool will execute
	*/

	template<typename F>
	void TaskScheduler::AddTask(F function)
	{
		AddTaskFunctor(new FunctorWithoutArgs<F>(function));
	}

	/*!
	* \brief Adds a task to the pending list
	*
	* \param function Task that the pool will execute
	* \param args Arguments of the function
	*/

	template<typename F, typename... Args>
	void TaskScheduler::AddTask(F function, Args&&... args)
	{
		AddTaskFunctor(new FunctorWithArgs<F, Args...>(function, std::forward<Args>(args)...));
	}

	/*!
	* \brief Adds a task to the pending list
	*
	* \param function Task that the pool will execute
	* \param object Object on which the method will be called
	*/

	template<typename C>
	void TaskScheduler::AddTask(void (C::*function)(), C* object)
	{
		AddTaskFunctor(new MemberWithoutArgs<C>(function, object));
	}
}

#include <Nazara/Core/DebugOff.hpp>

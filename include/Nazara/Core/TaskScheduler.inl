// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename F>
	void TaskScheduler::AddTask(F function)
	{
		AddTaskFunctor(new FunctorWithoutArgs<F>(function));
	}

	template<typename F, typename... Args>
	void TaskScheduler::AddTask(F function, Args&&... args)
	{
		AddTaskFunctor(new FunctorWithArgs<F, Args...>(function, std::forward<Args>(args)...));
	}

	template<typename C>
	void TaskScheduler::AddTask(void (C::*function)(), C* object)
	{
		AddTaskFunctor(new MemberWithoutArgs<C>(function, object));
	}
}

#include <Nazara/Core/DebugOff.hpp>

// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

template<typename F>
void NzTaskScheduler::AddTask(F function)
{
	AddTaskFunctor(new NzFunctorWithoutArgs<F>(function));
}

template<typename F, typename... Args>
void NzTaskScheduler::AddTask(F function, Args... args)
{
	AddTaskFunctor(new NzFunctorWithArgs<F, Args...>(function, args...));
}

template<typename C>
void NzTaskScheduler::AddTask(void (C::*function)(), C* object)
{
	AddTaskFunctor(new NzMemberWithoutArgs<C>(function, object));
}

#include <Nazara/Core/DebugOff.hpp>

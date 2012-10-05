// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

template<typename F>
NzThread::NzThread(F function) :
m_func(new NzFunctorWithoutArgs<F>(function))
{
}

template<typename F, typename... Args>
NzThread::NzThread(F function, Args... args) :
m_func(new NzFunctorWithArgs<F, Args...>(function, args...))
{
}

template<typename C>
NzThread::NzThread(void (C::*function)(), C* object) :
m_func(new NzMemberWithoutArgs<C>(function, object))
{
}

#include <Nazara/Core/DebugOff.hpp>

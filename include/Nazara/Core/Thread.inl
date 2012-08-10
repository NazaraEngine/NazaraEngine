// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

template<typename F> NzThread::NzThread(F function) :
m_func(new NzFunctorWithoutArgs<F>(function)),
m_impl(nullptr)
{
}

template<typename F, typename... Args> NzThread::NzThread(F function, Args... args) :
m_func(new NzFunctorWithArgs<F, Args...>(function, args...)),
m_impl(nullptr)
{
}

#include <Nazara/Core/DebugOff.hpp>

// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

template<typename F, typename... Args>
NzThread::NzThread(F function, Args... args)
{
	if (sizeof...(Args) == 0)
		CreateImpl(new NzFunctorWithoutArgs<F>(function));
	else
		CreateImpl(new NzFunctorWithArgs<F, Args...>(function, args...));
}

template<typename C>
NzThread::NzThread(void (C::*function)(), C* object)
{
	CreateImpl(new NzMemberWithoutArgs<C>(function, object));
}

#include <Nazara/Core/DebugOff.hpp>

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	template<typename F>
	Thread::Thread(F function)
	{
		CreateImpl(new FunctorWithoutArgs<F>(function));
	}

	template<typename F, typename... Args>
	Thread::Thread(F function, Args&&... args)
	{
		CreateImpl(new FunctorWithArgs<F, Args...>(function, std::forward<Args>(args)...));
	}

	template<typename C>
	Thread::Thread(void (C::*function)(), C* object)
	{
		CreateImpl(new MemberWithoutArgs<C>(function, object));
	}
}

#include <Nazara/Core/DebugOff.hpp>

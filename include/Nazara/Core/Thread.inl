// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Thread
	* \brief Core class that represents a thread
	*/

	/*!
	* \brief Constructs a Thread object with a function
	*
	* \param function Task the thread will execute in parallel
	*/

	template<typename F>
	Thread::Thread(F function)
	{
		CreateImpl(new FunctorWithoutArgs<F>(function));
	}

	/*!
	* \brief Constructs a Thread object with a function and its parameters
	*
	* \param function Task the thread will execute in parallel
	* \param args Arguments of the function
	*/

	template<typename F, typename... Args>
	Thread::Thread(F function, Args&&... args)
	{
		CreateImpl(new FunctorWithArgs<F, Args...>(function, std::forward<Args>(args)...));
	}

	/*!
	* \brief Constructs a Thread object with a member function and its object
	*
	* \param function Task the thread will execute in parallel
	* \param object Object on which the method will be called
	*/

	template<typename C>
	Thread::Thread(void (C::*function)(), C* object)
	{
		CreateImpl(new MemberWithoutArgs<C>(function, object));
	}
}

#include <Nazara/Core/DebugOff.hpp>

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Semaphore.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/SemaphoreImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/SemaphoreImpl.hpp>
#else
	#error Lack of implementation: Semaphore
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Semaphore
	* \brief Core class that represents a counting semaphore
	*/

	/*!
	* \brief Constructs a semaphore object with a count
	*/

	Semaphore::Semaphore(unsigned int count)
	{
		m_impl = new SemaphoreImpl(count);
	}

	/*!
	* \brief Destructs the object
	*/

	Semaphore::~Semaphore()
	{
		delete m_impl;
	}

	/*!
	* \brief Gets the number of count that can handle the semaphore
	* \return Number of count associated with the semaphore
	*/

	unsigned int Semaphore::GetCount() const
	{
		return m_impl->GetCount();
	}

	/*!
	* \brief Increments the count of the semaphore and wait if count equals zero
	*
	*  Increments the value of semaphore variable by 1. After the increment, if the pre-increment value was negative (meaning there are processes waiting for a resource), it transfers a blocked process from the semaphore's waiting queue to the ready queue
	*/

	void Semaphore::Post()
	{
		m_impl->Post();
	}

	/*!
	* \brief Decrements the count of the semaphore and wait if count equals zero
	*
	* If the value of semaphore variable is not negative, decrements it by 1. If the semaphore variable is now negative, the process executing Wait is blocked (i.e., added to the semaphore's queue) until the value is greater or equal to 1. Otherwise, the process continues execution, having used a unit of the resource
	*/

	void Semaphore::Wait()
	{
		m_impl->Wait();
	}

	/*!
	* \brief Decrements the count of the semaphore and wait if count equals zero
	* \return true if the semaphore successfully decrements before timeout
	*
	* If the value of semaphore variable is not negative, decrements it by 1. If the semaphore variable is now negative, the process executing Wait is blocked (i.e., added to the semaphore's queue) until the value is greater or equal to 1. Otherwise, the process continues execution, having used a unit of the resource
	*/

	bool Semaphore::Wait(UInt32 timeout)
	{
		return m_impl->Wait(timeout);
	}
}

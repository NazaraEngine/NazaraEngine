// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Mutex.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/ConditionVariableImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/ConditionVariableImpl.hpp>
#else
	#error Condition variable has no implementation
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::ConditionVariable
	* \brief Core class that represents a condition variable
	*
	* The ConditionVariable class is a synchronization primitive that can be used to block a thread, or multiple threads at the same time, until another thread both modifies a shared variable (the condition), and notifies the ConditionVariable
	*/

	/*!
	* \brief Constructs a ConditionVariable object by default
	*/

	ConditionVariable::ConditionVariable()
	{
		m_impl = new ConditionVariableImpl;
	}

	/*!
	* \brief Destructs the object
	*/

	ConditionVariable::~ConditionVariable()
	{
		delete m_impl;
	}

	/*!
	* \brief Sends a signal to one thread waiting on the condition
	*
	* If any threads are waiting on *this, calling Signal unblocks one of the waiting threads
	*
	* \see SignalAll
	*/

	void ConditionVariable::Signal()
	{
		m_impl->Signal();
	}

	/*!
	* \brief Sends a signal to every threads waiting on the condition
	*
	* Unblocks all threads currently waiting for *this
	*
	* \see Signal
	*/

	void ConditionVariable::SignalAll()
	{
		m_impl->SignalAll();
	}

	/*!
	* \brief Makes the thread wait on the condition
	*
	* Wait causes the current thread to block until the condition variable is notified or a spurious wakeup occurs
	*
	* \param mutex Mutex for the condition
	*
	* \remark Produces a NazaraAssert if mutex is invalid
	*/

	void ConditionVariable::Wait(Mutex* mutex)
	{
		NazaraAssert(mutex != nullptr, "Mutex must be valid");
		m_impl->Wait(mutex->m_impl);
	}

	/*!
	* \brief Makes the thread wait on the condition for a certain amount of time
	*
	* Wait causes the current thread to block until the condition variable is notified, a specific time is reached, or a spurious wakeup occurs
	*
	* \param mutex Mutex for the condition
	* \param timeout Time before expiration of the waiting
	*
	* \remark Produces a NazaraAssert if mutex is invalid
	*/

	bool ConditionVariable::Wait(Mutex* mutex, UInt32 timeout)
	{
		NazaraAssert(mutex != nullptr, "Mutex must be valid");
		return m_impl->Wait(mutex->m_impl, timeout);
	}
}

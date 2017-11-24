// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Error.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/MutexImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/MutexImpl.hpp>
#else
	#error Lack of implementation: Mutex
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Mutex
	* \brief Core class that represents a binary semaphore, a mutex
	*
	* \remark The mutex is recursive, it means that a thread who owns the mutex can call the same function which needs the same mutex
	*/

	/*!
	* \brief Constructs a Mutex object by default
	*/

	Mutex::Mutex()
	{
		m_impl = new MutexImpl;
	}

	/*!
	* \brief Destructs the object
	*/

	Mutex::~Mutex()
	{
		delete m_impl;
	}

	/*!
	* \brief Locks the mutex
	*
	* If another thread has already locked the mutex, a call to lock will block execution until the lock is acquired. A thread may call lock on a recursive mutex repeatedly. Ownership will only be released after the thread makes a matching number of calls to unlock
	*/

	void Mutex::Lock()
	{
		NazaraAssert(m_impl, "Cannot lock a moved mutex");
		m_impl->Lock();
	}

	/*!
	* \brief Tries to lock the mutex
	* \return true if the lock was acquired successfully
	*/

	bool Mutex::TryLock()
	{
		NazaraAssert(m_impl, "Cannot lock a moved mutex");
		return m_impl->TryLock();
	}

	/*!
	* \brief Unlocks the mutex
	*
	* Unlocks the mutex if its level of ownership is 1 (there was exactly one more call to Lock() than there were calls to Unlock() made by this thread), reduces the level of ownership by 1 otherwise
	*/

	void Mutex::Unlock()
	{
		NazaraAssert(m_impl, "Cannot unlock a moved mutex");
		m_impl->Unlock();
	}
}

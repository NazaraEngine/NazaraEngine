// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::LockGuard
	* \brief Core class that represents a mutex wrapper that provides a convenient RAII-style mechanism
	*/

	/*!
	* \brief Constructs a LockGuard object with a mutex
	*
	* \param mutex Mutex to lock
	* \param lock Should the mutex be locked by the constructor
	*/
	inline LockGuard::LockGuard(Mutex& mutex, bool lock) :
	m_mutex(mutex),
	m_locked(false)
	{
		if (lock)
		{
			m_mutex.Lock();
			m_locked = true;
		}
	}

	/*!
	* \brief Destructs a LockGuard object and unlocks the mutex if it was previously locked
	*/
	inline LockGuard::~LockGuard()
	{
		if (m_locked)
			m_mutex.Unlock();
	}

	/*!
	* \brief Locks the underlying mutex
	*
	* \see Mutex::Lock
	*/
	inline void LockGuard::Lock()
	{
		NazaraAssert(!m_locked, "Mutex is already locked");

		m_mutex.Lock();
	}

	/*!
	* \brief Tries to lock the underlying mutex
	* 
	* \see Mutex::TryLock
	* 
	* \return true if the lock was acquired successfully
	*/
	inline bool LockGuard::TryLock()
	{
		NazaraAssert(!m_locked, "Mutex is already locked");

		return m_mutex.TryLock();
	}

	/*!
	* \brief Unlocks the underlying mutex
	*
	* \see Mutex::Unlock
	*/
	inline void LockGuard::Unlock()
	{
		NazaraAssert(m_locked, "Mutex is not locked");

		m_mutex.Unlock();
	}
}

#include <Nazara/Core/DebugOff.hpp>

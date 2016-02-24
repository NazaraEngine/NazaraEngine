// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \class Nz::LockGuard
	* \brief Core class that represents a mutex wrapper that provides a convenient RAII-style mechanism
	*/

	/*!
	* \brief Constructs a LockGuard object with a mutex
	*
	* \param mutex Mutex to lock
	*/

	LockGuard::LockGuard(Mutex& mutex) :
	m_mutex(mutex)
	{
		m_mutex.Lock();
	}

	/*!
	* \brief Destructs a LockGuard object and unlocks the mutex
	*/

	LockGuard::~LockGuard()
	{
		m_mutex.Unlock();
	}
}

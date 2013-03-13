// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/LockGuard.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Debug.hpp>

NzLockGuard::NzLockGuard(NzMutex& mutex) :
m_mutex(mutex)
{
	m_mutex.Lock();
}

NzLockGuard::~NzLockGuard()
{
	m_mutex.Unlock();
}

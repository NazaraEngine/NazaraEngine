// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#define NAZARA_LOCK_CPP

#include <Nazara/Core/Lock.hpp>
#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Debug.hpp>

NzLock::NzLock(NzMutex& mutex) :
m_mutex(mutex)
{
	m_mutex.Lock();
}

NzLock::~NzLock()
{
	m_mutex.Unlock();
}

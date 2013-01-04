// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/MutexImpl.hpp>
#include <Nazara/Core/Debug.hpp>

NzMutexImpl::NzMutexImpl()
{
    
	pthread_mutex_init(&m_pmutex, NULL);
}

NzMutexImpl::~NzMutexImpl()
{
    pthread_mutex_
}

void NzMutexImpl::Lock()
{
	pthread_mutex_lock(&m_pmutex);
}

bool NzMutexImpl::TryLock()
{
    pthread_mutex_trylock(&m_pmutex) == 0;
}

void NzMutexImpl::Unlock()
{
	pthread_mutex_unlock(&m_pmutex);
}

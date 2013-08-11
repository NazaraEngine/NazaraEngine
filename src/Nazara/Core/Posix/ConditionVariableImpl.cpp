// Copyright (C) 2013 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/ConditionVariableImpl.hpp>
#include <Nazara/Core/Posix/MutexImpl.hpp>
#include <Nazara/Core/Debug.hpp>

NzConditionVariableImpl::NzConditionVariableImpl()
{
	pthread_cond_init(&m_cv, nullptr);
}

NzConditionVariableImpl::~NzConditionVariableImpl()
{
    pthread_cond_destroy(&m_cv);
}

void NzConditionVariableImpl::Signal()
{
    pthread_cond_signal(&m_cv);
}

void NzConditionVariableImpl::SignalAll()
{
    pthread_cond_broadcast(&m_cv);
}

void NzConditionVariableImpl::Wait(NzMutexImpl* mutex)
{
	pthread_cond_wait(&m_cv, &mutex->m_handle);
}

bool NzConditionVariableImpl::Wait(NzMutexImpl* mutex, nzUInt32 timeout)
{
	// get the current time
	timeval tv;
	gettimeofday(&tv, NULL);

	// construct the time limit (current time + time to wait)
	timespec ti;
	ti.tv_nsec = (tv.tv_usec + (timeout % 1000)) * 1000000;
	ti.tv_sec = tv.tv_sec + (timeout / 1000) + (ti.tv_nsec / 1000000000);
	ti.tv_nsec %= 1000000000;

	return pthread_cond_timedwait(&m_cv,&mutex->m_handle, &ti) != 0;
}

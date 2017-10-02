// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/ConditionVariableImpl.hpp>
#include <Nazara/Core/Posix/MutexImpl.hpp>
#include <sys/time.h>
#include <time.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	ConditionVariableImpl::ConditionVariableImpl()
	{
		pthread_cond_init(&m_cv, nullptr);
	}

	ConditionVariableImpl::~ConditionVariableImpl()
	{
		pthread_cond_destroy(&m_cv);
	}

	void ConditionVariableImpl::Signal()
	{
		pthread_cond_signal(&m_cv);
	}

	void ConditionVariableImpl::SignalAll()
	{
		pthread_cond_broadcast(&m_cv);
	}

	void ConditionVariableImpl::Wait(MutexImpl* mutex)
	{
		pthread_cond_wait(&m_cv, &mutex->m_handle);
	}

	bool ConditionVariableImpl::Wait(MutexImpl* mutex, UInt32 timeout)
	{
		// get the current time
		timeval tv;
		gettimeofday(&tv, nullptr);

		// construct the time limit (current time + time to wait)
		timespec ti;
		ti.tv_nsec = (tv.tv_usec + (timeout % 1000)) * 1000000;
		ti.tv_sec = tv.tv_sec + (timeout / 1000) + (ti.tv_nsec / 1000000000);
		ti.tv_nsec %= 1000000000;

		return pthread_cond_timedwait(&m_cv,&mutex->m_handle, &ti) != 0;
	}
}

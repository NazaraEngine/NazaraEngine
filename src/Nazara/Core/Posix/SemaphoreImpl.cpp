// Copyright (C) 2013 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/SemaphoreImpl.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <limits>
#include <Nazara/Core/Debug.hpp>
#include <sys/time.h>

NzSemaphoreImpl::NzSemaphoreImpl(unsigned int count)
{
	if(sem_init(&m_semaphore, 0, count) != 0)
		NazaraError("Failed to create semaphore: " + NzGetLastSystemError());
}

NzSemaphoreImpl::~NzSemaphoreImpl()
{
	sem_destroy(&m_semaphore);
}

unsigned int NzSemaphoreImpl::GetCount() const
{
	int count=0;
	sem_getvalue(const_cast<sem_t*>(&m_semaphore), &count);
	return static_cast<unsigned int>(count);
}

void NzSemaphoreImpl::Post()
{
	#if NAZARA_CORE_SAFE
	if (sem_post(&m_semaphore)==-1)
		NazaraError("Failed to release semaphore: " + NzGetLastSystemError());
	#else
	sem_post(&m_semaphore);
	#endif
}

void NzSemaphoreImpl::Wait()
{
	#if NAZARA_CORE_SAFE
	if (sem_wait(&m_semaphore) == -1 )
		NazaraError("Failed to wait for semaphore: " + NzGetLastSystemError());
	#else
	sem_wait(&m_semaphore);
	#endif
}

bool NzSemaphoreImpl::Wait(nzUInt32 timeout)
{
    timeval tv;
    gettimeofday(&tv, nullptr);

	timespec ti;
	ti.tv_nsec = (tv.tv_usec + (timeout % 1000)) * 1000000;
	ti.tv_sec = tv.tv_sec + (timeout / 1000) + (ti.tv_nsec / 1000000000);
	ti.tv_nsec %= 1000000000;

	#if NAZARA_CORE_SAFE
	if (sem_timedwait(&m_semaphore, &ti) != 0)
	{
		NazaraError("Failed to wait for semaphore: " + NzGetLastSystemError());
		return false;
	}

	return true;
	#else
		return sem_timedwait(&m_semaphore, &ti) != 0;
	#endif
}

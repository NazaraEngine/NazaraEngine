// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Posix/SemaphoreImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <time.h>
#include <sys/time.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	SemaphoreImpl::SemaphoreImpl(unsigned int count)
	{
		if (sem_init(&m_semaphore, 0, count) != 0)
			NazaraError("Failed to create semaphore: " + Error::GetLastSystemError());
	}

	SemaphoreImpl::~SemaphoreImpl()
	{
		sem_destroy(&m_semaphore);
	}

	unsigned int SemaphoreImpl::GetCount() const
	{
		int count=0;
		sem_getvalue(const_cast<sem_t*>(&m_semaphore), &count);
		return static_cast<unsigned int>(count);
	}

	void SemaphoreImpl::Post()
	{
		#if NAZARA_CORE_SAFE
		if (sem_post(&m_semaphore)==-1)
			NazaraError("Failed to release semaphore: " + Error::GetLastSystemError());
		#else
		sem_post(&m_semaphore);
		#endif
	}

	void SemaphoreImpl::Wait()
	{
		#if NAZARA_CORE_SAFE
		if (sem_wait(&m_semaphore) == -1 )
			NazaraError("Failed to wait for semaphore: " + Error::GetLastSystemError());
		#else
		sem_wait(&m_semaphore);
		#endif
	}

	bool SemaphoreImpl::Wait(UInt32 timeout)
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
			NazaraError("Failed to wait for semaphore: " + Error::GetLastSystemError());
			return false;
		}

		return true;
		#else
		return sem_timedwait(&m_semaphore, &ti) != 0;
		#endif
	}
}

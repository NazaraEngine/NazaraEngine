// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/SemaphoreImpl.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <limits>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	SemaphoreImpl::SemaphoreImpl(unsigned int count)
	{
		m_semaphore = CreateSemaphoreW(nullptr, count, std::numeric_limits<LONG>::max(), nullptr);
		if (!m_semaphore)
			NazaraError("Failed to create semaphore: " + Error::GetLastSystemError());
	}

	SemaphoreImpl::~SemaphoreImpl()
	{
		CloseHandle(m_semaphore);
	}

	unsigned int SemaphoreImpl::GetCount() const
	{
		LONG count;
		ReleaseSemaphore(m_semaphore, 0, &count);
		return count;
	}

	void SemaphoreImpl::Post()
	{
		#if NAZARA_CORE_SAFE
		if (!ReleaseSemaphore(m_semaphore, 1, nullptr))
			NazaraError("Failed to release semaphore: " + Error::GetLastSystemError());
		#else
		ReleaseSemaphore(m_semaphore, 1, nullptr);
		#endif
	}

	void SemaphoreImpl::Wait()
	{
		#if NAZARA_CORE_SAFE
		if (WaitForSingleObject(m_semaphore, INFINITE) == WAIT_FAILED)
			NazaraError("Failed to wait for semaphore: " + Error::GetLastSystemError());
		#else
		WaitForSingleObject(m_semaphore, INFINITE);
		#endif
	}

	bool SemaphoreImpl::Wait(UInt32 timeout)
	{
		#if NAZARA_CORE_SAFE
		DWORD result = WaitForSingleObject(m_semaphore, timeout);
		if (result == WAIT_FAILED)
		{
			NazaraError("Failed to wait for semaphore: " + Error::GetLastSystemError());
			return false;
		}
		else
			return result == WAIT_OBJECT_0;
		#else
		return WaitForSingleObject(m_semaphore, timeout) == WAIT_OBJECT_0;
		#endif
	}
}

// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/SemaphoreImpl.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <limits>
#include <Nazara/Core/Debug.hpp>

NzSemaphoreImpl::NzSemaphoreImpl(unsigned int count)
{
	m_semaphore = CreateSemaphore(nullptr, count, std::numeric_limits<LONG>::max(), nullptr);
	if (!m_semaphore)
		NazaraError("Failed to create semaphore: " + NzError::GetLastSystemError());
}

NzSemaphoreImpl::~NzSemaphoreImpl()
{
	CloseHandle(m_semaphore);
}

unsigned int NzSemaphoreImpl::GetCount() const
{
	LONG count;
	ReleaseSemaphore(m_semaphore, 0, &count);
	return count;
}

void NzSemaphoreImpl::Post()
{
	#if NAZARA_CORE_SAFE
	if (!ReleaseSemaphore(m_semaphore, 1, nullptr))
		NazaraError("Failed to release semaphore: " + NzError::GetLastSystemError());
	#else
	ReleaseSemaphore(m_semaphore, 1, nullptr);
	#endif
}

void NzSemaphoreImpl::Wait()
{
	#if NAZARA_CORE_SAFE
	if (WaitForSingleObject(m_semaphore, INFINITE) == WAIT_FAILED)
		NazaraError("Failed to wait for semaphore: " + NzError::GetLastSystemError());
	#else
	WaitForSingleObject(m_semaphore, INFINITE);
	#endif
}

bool NzSemaphoreImpl::Wait(nzUInt32 timeout)
{
	#if NAZARA_CORE_SAFE
	DWORD result = WaitForSingleObject(m_semaphore, timeout);
	if (result == WAIT_FAILED)
	{
		NazaraError("Failed to wait for semaphore: " + NzError::GetLastSystemError());
		return false;
	}
	else
		return result == WAIT_OBJECT_0;
	#else
	return WaitForSingleObject(m_semaphore, timeout) == WAIT_OBJECT_0;
	#endif
}

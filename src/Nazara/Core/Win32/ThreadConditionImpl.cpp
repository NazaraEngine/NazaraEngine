// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Source: http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

#include <Nazara/Core/Win32/ThreadConditionImpl.hpp>
#include <Nazara/Core/Win32/MutexImpl.hpp>
#include <Nazara/Core/Debug.hpp>

NzThreadConditionImpl::NzThreadConditionImpl()
{
	#ifdef NAZARA_PLATFORM_WINDOWSVISTA
	InitializeConditionVariable(&m_cv);
	#else
	m_count = 0;
	InitializeCriticalSection(&m_countLock);
	m_events[SIGNAL] = CreateEvent(nullptr, false, false, nullptr); // auto-reset event
	m_events[BROADCAST] = CreateEvent(nullptr, true, false, nullptr); // manual-reset event
	#endif
}

#ifndef NAZARA_PLATFORM_WINDOWSVISTA
NzThreadConditionImpl::~NzThreadConditionImpl()
{
	DeleteCriticalSection(&m_countLock);
}
#endif

void NzThreadConditionImpl::Signal()
{
	#ifdef NAZARA_PLATFORM_WINDOWSVISTA
	WakeConditionVariable(&m_cv);
	#else
	// Avoid race conditions.
	EnterCriticalSection(&m_countLock);
	bool haveWaiters = (m_count > 0);
	LeaveCriticalSection(&m_countLock);

	if (haveWaiters)
		SetEvent(m_events[SIGNAL]);
	#endif
}

void NzThreadConditionImpl::SignalAll()
{
	#ifdef NAZARA_PLATFORM_WINDOWSVISTA
	WakeAllConditionVariable(&m_cv);
	#else
	// Avoid race conditions.
	EnterCriticalSection(&m_countLock);
	bool haveWaiters = (m_count > 0);
	LeaveCriticalSection (&m_countLock);

	if (haveWaiters)
		SetEvent(m_events[BROADCAST]);
	#endif
}

void NzThreadConditionImpl::Wait(NzMutexImpl* mutex)
{
	Wait(mutex, INFINITE);
}

bool NzThreadConditionImpl::Wait(NzMutexImpl* mutex, nzUInt32 timeout)
{
	#ifdef NAZARA_PLATFORM_WINDOWSVISTA
	return SleepConditionVariableCS(&m_cv, mutex->m_criticalSection, timeout);
	#else
	// Avoid race conditions.
	EnterCriticalSection(&m_countLock);
	m_count++;
	LeaveCriticalSection(&m_countLock);

	// It's ok to release the mutex here since Win32
	// manual-reset events maintain state when used with SetEvent.
	// This avoids the "lost wakeup" bug...
	LeaveCriticalSection(&mutex->m_criticalSection);

	// Wait for either event to become signaled due to Signal being called or SignalAll being called.
	int result = WaitForMultipleObjects(2, m_events, false, timeout);

	EnterCriticalSection(&m_countLock);
	m_count--;
	bool lastWaiter = (result == WAIT_OBJECT_0 + BROADCAST && m_count == 0);
	LeaveCriticalSection(&m_countLock);

	// Some thread called SignalAll
	if (lastWaiter)
		// We're the last waiter to be notified or to stop waiting, so reset the manual event.
		ResetEvent(m_events[BROADCAST]);

	// Reacquire the mutex.
	EnterCriticalSection(&mutex->m_criticalSection);

	return result != WAIT_TIMEOUT;
	#endif
}

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Source: http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

#include <Nazara/Core/Win32/ConditionVariableImpl.hpp>
#include <Nazara/Core/Win32/MutexImpl.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	ConditionVariableImpl::ConditionVariableImpl()
	{
		#if NAZARA_CORE_WINDOWS_NT6
		InitializeConditionVariable(&m_cv);
		#else
		m_count = 0;
		m_events[BROADCAST] = CreateEvent(nullptr, true, false, nullptr); // manual-reset event
		m_events[SIGNAL] = CreateEvent(nullptr, false, false, nullptr); // auto-reset event
		#endif
	}

	#if !NAZARA_CORE_WINDOWS_NT6
	ConditionVariableImpl::~ConditionVariableImpl()
	{
		CloseHandle(m_events[BROADCAST]);
		CloseHandle(m_events[SIGNAL]);
	}
	#endif

	void ConditionVariableImpl::Signal()
	{
		#if NAZARA_CORE_WINDOWS_NT6
		WakeConditionVariable(&m_cv);
		#else
		if (m_count > 0)
			SetEvent(m_events[SIGNAL]);
		#endif
	}

	void ConditionVariableImpl::SignalAll()
	{
		#if NAZARA_CORE_WINDOWS_NT6
		WakeAllConditionVariable(&m_cv);
		#else
		if (m_count > 0)
			SetEvent(m_events[BROADCAST]);
		#endif
	}

	void ConditionVariableImpl::Wait(MutexImpl* mutex)
	{
		Wait(mutex, INFINITE);
	}

	bool ConditionVariableImpl::Wait(MutexImpl* mutex, UInt32 timeout)
	{
		#if NAZARA_CORE_WINDOWS_NT6
		return SleepConditionVariableCS(&m_cv, &mutex->m_criticalSection, timeout) == TRUE;
		#else
		m_count++;

		// It's ok to release the mutex here since Win32
		// manual-reset events maintain state when used with SetEvent.
		// This avoids the "lost wakeup" bug...
		LeaveCriticalSection(&mutex->m_criticalSection);

		// Wait for either event to become signaled due to Signal being called or SignalAll being called.
		int result = WaitForMultipleObjects(2, m_events, false, timeout);

		// Some thread called SignalAll
		if (--m_count == 0 && result == WAIT_OBJECT_0 + BROADCAST)
			// We're the last waiter to be notified or to stop waiting, so reset the manual event.
			ResetEvent(m_events[BROADCAST]);

		// Reacquire the mutex.
		EnterCriticalSection(&mutex->m_criticalSection);

		return result != WAIT_TIMEOUT;
		#endif
	}

}

// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/MutexImpl.hpp>
#include <Nazara/Core/Debug.hpp>

NzMutexImpl::NzMutexImpl()
{
	#if NAZARA_CORE_WINDOWS_CS_SPINLOCKS > 0
	InitializeCriticalSectionAndSpinCount(&m_criticalSection, NAZARA_CORE_WINDOWS_CS_SPINLOCKS);
	#else
	InitializeCriticalSection(&m_criticalSection);
	#endif
}

NzMutexImpl::~NzMutexImpl()
{
	DeleteCriticalSection(&m_criticalSection);
}

void NzMutexImpl::Lock()
{
	EnterCriticalSection(&m_criticalSection);
}

bool NzMutexImpl::TryLock()
{
	return TryEnterCriticalSection(&m_criticalSection) != 0;
}

void NzMutexImpl::Unlock()
{
	LeaveCriticalSection(&m_criticalSection);
}

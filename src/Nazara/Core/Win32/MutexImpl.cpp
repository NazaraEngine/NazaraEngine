// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/MutexImpl.hpp>
#include <Nazara/Core/Debug.hpp>

NzMutexImpl::NzMutexImpl()
{
	InitializeCriticalSection(&m_criticalSection);
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

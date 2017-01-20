// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/MutexImpl.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	MutexImpl::MutexImpl()
	{
		#if NAZARA_CORE_WINDOWS_CS_SPINLOCKS > 0
		InitializeCriticalSectionAndSpinCount(&m_criticalSection, NAZARA_CORE_WINDOWS_CS_SPINLOCKS);
		#else
		InitializeCriticalSection(&m_criticalSection);
		#endif
	}

	MutexImpl::~MutexImpl()
	{
		DeleteCriticalSection(&m_criticalSection);
	}

	void MutexImpl::Lock()
	{
		EnterCriticalSection(&m_criticalSection);
	}

	bool MutexImpl::TryLock()
	{
		return TryEnterCriticalSection(&m_criticalSection) != 0;
	}

	void MutexImpl::Unlock()
	{
		LeaveCriticalSection(&m_criticalSection);
	}
}

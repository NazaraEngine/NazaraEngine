// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#define NAZARA_MUTEX_CPP

#include <Nazara/Core/Mutex.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/MutexImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/MutexImpl.hpp>
#else
	#error Lack of implementation: Mutex
#endif

#include <Nazara/Core/Debug.hpp>

NzMutex::NzMutex()
{
	m_impl = new NzMutexImpl;
}

NzMutex::~NzMutex()
{
	delete m_impl;
}

void NzMutex::Lock()
{
	m_impl->Lock();
}

bool NzMutex::TryLock()
{
	return m_impl->TryLock();
}

void NzMutex::Unlock()
{
	m_impl->Unlock();
}

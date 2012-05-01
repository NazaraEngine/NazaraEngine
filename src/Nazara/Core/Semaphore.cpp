// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#define NAZARA_SEMAPHORE_CPP

#include <Nazara/Core/Semaphore.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/SemaphoreImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/SemaphoreImpl.hpp>
#else
	#error Lack of implementation: Semaphore
#endif

#include <Nazara/Core/Debug.hpp>

NzSemaphore::NzSemaphore(unsigned int count)
{
	m_impl = new NzSemaphoreImpl(count);
}

NzSemaphore::~NzSemaphore()
{
	delete m_impl;
}

unsigned int NzSemaphore::GetCount() const
{
	return m_impl->GetCount();
}

void NzSemaphore::Post()
{
	m_impl->Post();
}

void NzSemaphore::Wait()
{
	m_impl->Wait();
}

bool NzSemaphore::Wait(nzUInt32 timeout)
{
	return m_impl->Wait(timeout);
}

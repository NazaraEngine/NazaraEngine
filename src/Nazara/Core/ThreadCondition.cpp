// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ThreadCondition.hpp>
#include <Nazara/Core/Mutex.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/ThreadConditionImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/ThreadConditionImpl.hpp>
#else
	#error Thread condition has no implementation
#endif

#include <Nazara/Core/Debug.hpp>

NzThreadCondition::NzThreadCondition()
{
	m_impl = new NzThreadConditionImpl;
}

NzThreadCondition::~NzThreadCondition()
{
	delete m_impl;
}

void NzThreadCondition::Signal()
{
	m_impl->Signal();
}

void NzThreadCondition::SignalAll()
{
	m_impl->SignalAll();
}

void NzThreadCondition::Wait(NzMutex* mutex)
{
	m_impl->Wait(mutex->m_impl);
}

bool NzThreadCondition::Wait(NzMutex* mutex, nzUInt32 timeout)
{
	return m_impl->Wait(mutex->m_impl, timeout);
}

// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Mutex.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/ConditionVariableImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/ConditionVariableImpl.hpp>
#else
	#error Thread condition has no implementation
#endif

#include <Nazara/Core/Debug.hpp>

NzConditionVariable::NzConditionVariable()
{
	m_impl = new NzConditionVariableImpl;
}

NzConditionVariable::~NzConditionVariable()
{
	delete m_impl;
}

void NzConditionVariable::Signal()
{
	m_impl->Signal();
}

void NzConditionVariable::SignalAll()
{
	m_impl->SignalAll();
}

void NzConditionVariable::Wait(NzMutex* mutex)
{
	m_impl->Wait(mutex->m_impl);
}

bool NzConditionVariable::Wait(NzMutex* mutex, nzUInt32 timeout)
{
	return m_impl->Wait(mutex->m_impl, timeout);
}

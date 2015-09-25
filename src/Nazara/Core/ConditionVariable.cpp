// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Mutex.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/ConditionVariableImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/ConditionVariableImpl.hpp>
#else
	#error Condition variable has no implementation
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	ConditionVariable::ConditionVariable()
	{
		m_impl = new ConditionVariableImpl;
	}

	ConditionVariable::~ConditionVariable()
	{
		delete m_impl;
	}

	void ConditionVariable::Signal()
	{
		m_impl->Signal();
	}

	void ConditionVariable::SignalAll()
	{
		m_impl->SignalAll();
	}

	void ConditionVariable::Wait(Mutex* mutex)
	{
		NazaraAssert(mutex != nullptr, "Mutex must be valid");
		m_impl->Wait(mutex->m_impl);
	}

	bool ConditionVariable::Wait(Mutex* mutex, UInt32 timeout)
	{
		NazaraAssert(mutex != nullptr, "Mutex must be valid");
		return m_impl->Wait(mutex->m_impl, timeout);
	}
}

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Semaphore.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/SemaphoreImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/SemaphoreImpl.hpp>
#else
	#error Lack of implementation: Semaphore
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	Semaphore::Semaphore(unsigned int count)
	{
		m_impl = new SemaphoreImpl(count);
	}

	Semaphore::~Semaphore()
	{
		delete m_impl;
	}

	unsigned int Semaphore::GetCount() const
	{
		return m_impl->GetCount();
	}

	void Semaphore::Post()
	{
		m_impl->Post();
	}

	void Semaphore::Wait()
	{
		m_impl->Wait();
	}

	bool Semaphore::Wait(UInt32 timeout)
	{
		return m_impl->Wait(timeout);
	}
}

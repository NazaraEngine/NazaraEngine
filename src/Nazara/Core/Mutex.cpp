// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Mutex.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/MutexImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/MutexImpl.hpp>
#else
	#error Lack of implementation: Mutex
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	Mutex::Mutex()
	{
		m_impl = new MutexImpl;
	}

	Mutex::~Mutex()
	{
		delete m_impl;
	}

	void Mutex::Lock()
	{
		m_impl->Lock();
	}

	bool Mutex::TryLock()
	{
		return m_impl->TryLock();
	}

	void Mutex::Unlock()
	{
		m_impl->Unlock();
	}
}

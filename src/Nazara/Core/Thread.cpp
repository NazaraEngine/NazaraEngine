// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Inspiré du code de la SFML par Laurent Gomila

#include <Nazara/Core/Thread.hpp>
#include <Nazara/Core/Error.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/ThreadImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/ThreadImpl.hpp>
#else
	#error Thread has no implementation
#endif

#include <Nazara/Core/Debug.hpp>

NzThread::~NzThread()
{
	if (!m_independent)
		Join();
	else
		delete m_impl;
}

NzThread::Id NzThread::GetId() const
{
	if (m_impl)
		return m_impl->GetId();
	else
		return NzThread::Id();
}

bool NzThread::IsCurrent() const
{
	if (m_impl)
		return m_impl->IsCurrent();
	else
		return false;
}

void NzThread::Launch(bool independent)
{
	Join();
	m_independent = independent;
	m_impl = new NzThreadImpl(this);
}

void NzThread::Join()
{
	if (m_impl)
	{
		#if NAZARA_CORE_SAFE
		if (m_impl->IsCurrent())
		{
			NazaraError("A thread cannot join itself");
			return;
		}
		#endif

		m_impl->Join();
		delete m_impl;
		m_impl = nullptr;
	}
}

void NzThread::Terminate()
{
	if (m_impl)
	{
		m_impl->Terminate();
		delete m_impl;
		m_impl = nullptr;
	}
}


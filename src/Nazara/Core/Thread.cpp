// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Thread.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <stdexcept>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/ThreadImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/ThreadImpl.hpp>
#else
	#error Thread has no implementation
#endif

#include <Nazara/Core/Debug.hpp>

/*********************************NzThread::Id********************************/

bool operator==(const NzThread::Id& lhs, const NzThread::Id& rhs)
{
	return lhs.m_id == rhs.m_id;
}

bool operator!=(const NzThread::Id& lhs, const NzThread::Id& rhs)
{
	return lhs.m_id != rhs.m_id;
}

bool operator<(const NzThread::Id& lhs, const NzThread::Id& rhs)
{
	return lhs.m_id < rhs.m_id;
}

bool operator<=(const NzThread::Id& lhs, const NzThread::Id& rhs)
{
	return lhs.m_id <= rhs.m_id;
}

bool operator>(const NzThread::Id& lhs, const NzThread::Id& rhs)
{
	return lhs.m_id > rhs.m_id;
}

bool operator>=(const NzThread::Id& lhs, const NzThread::Id& rhs)
{
	return lhs.m_id >= rhs.m_id;
}

bool operator<<(std::ostream& o, const NzThread::Id& id)
{
	o << id.m_id;
	return o;
}

NzThread::Id::Id(NzThreadImpl* thread) :
m_id(thread)
{
}

/***********************************NzThread**********************************/

NzThread::NzThread() :
m_impl(nullptr)
{
}

NzThread::NzThread(NzThread&& other) :
m_impl(other.m_impl)
{
	other.m_impl = nullptr;
}

NzThread::~NzThread()
{
	if (m_impl)
	{
		m_impl->Join();
		delete m_impl;
		m_impl = nullptr;
	}
}

void NzThread::Detach()
{
	if (m_impl)
	{
		m_impl->Detach();
		delete m_impl;
		m_impl = nullptr;
	}
}

NzThread::Id NzThread::GetId() const
{
	return NzThread::Id(m_impl);
}

bool NzThread::IsJoinable() const
{
	return m_impl != nullptr;
}

void NzThread::Join()
{
	#if NAZARA_CORE_SAFE
	if (!m_impl)
	{
		NazaraError("This thread is not joinable");
		return;
	}
	#endif

	m_impl->Join();
	delete m_impl;
	m_impl = nullptr;
}

NzThread& NzThread::operator=(NzThread&& thread)
{
	#if NAZARA_CORE_SAFE
	if (m_impl)
	{
		NazaraError("This thread cannot be joined");
		std::terminate();
	}
	#endif

	std::swap(m_impl, thread.m_impl);
	return *this;
}

unsigned int NzThread::HardwareConcurrency()
{
	return NzHardwareInfo::GetProcessorCount();
}

void NzThread::Sleep(nzUInt32 milliseconds)
{
	NzThreadImpl::Sleep(milliseconds);
}

void NzThread::CreateImpl(NzFunctor* functor)
{
	m_impl = new NzThreadImpl(functor);
}

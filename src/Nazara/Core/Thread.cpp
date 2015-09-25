// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Thread.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <ostream>
#include <stdexcept>

#if defined(NAZARA_PLATFORM_WINDOWS)
	#include <Nazara/Core/Win32/ThreadImpl.hpp>
#elif defined(NAZARA_PLATFORM_POSIX)
	#include <Nazara/Core/Posix/ThreadImpl.hpp>
#else
	#error Thread has no implementation
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	Thread::Thread() :
	m_impl(nullptr)
	{
	}

	Thread::Thread(Thread&& other) noexcept :
	m_impl(other.m_impl)
	{
		other.m_impl = nullptr;
	}

	Thread::~Thread()
	{
		if (m_impl)
		{
			m_impl->Join();
			delete m_impl;
			m_impl = nullptr;
		}
	}

	void Thread::Detach()
	{
		if (m_impl)
		{
			m_impl->Detach();
			delete m_impl;
			m_impl = nullptr;
		}
	}

	Thread::Id Thread::GetId() const
	{
		return Thread::Id(m_impl);
	}

	bool Thread::IsJoinable() const
	{
		return m_impl != nullptr;
	}

	void Thread::Join()
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

	Thread& Thread::operator=(Thread&& thread)
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

	unsigned int Thread::HardwareConcurrency()
	{
		return HardwareInfo::GetProcessorCount();
	}

	void Thread::Sleep(UInt32 milliseconds)
	{
		ThreadImpl::Sleep(milliseconds);
	}

	void Thread::CreateImpl(Functor* functor)
	{
		m_impl = new ThreadImpl(functor);
	}

	/*********************************Thread::Id********************************/

	Thread::Id::Id(ThreadImpl* thread) :
	m_id(thread)
	{
	}

	bool operator==(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id == rhs.m_id;
	}

	bool operator!=(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id != rhs.m_id;
	}

	bool operator<(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id < rhs.m_id;
	}

	bool operator<=(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id <= rhs.m_id;
	}

	bool operator>(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id > rhs.m_id;
	}

	bool operator>=(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id >= rhs.m_id;
	}

	std::ostream& operator<<(std::ostream& o, const Nz::Thread::Id& id)
	{
		o << id.m_id;
		return o;
	}
}

// Copyright (C) 2017 Jérôme Leclercq
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
	/*!
	* \ingroup core
	* \class Nz::Thread
	* \brief Core class that represents a thread
	*/

	/*!
	* \brief Constructs a Thread<T> object by default
	*/

	Thread::Thread() :
	m_impl(nullptr)
	{
	}

	/*!
	* \brief Constructs a Thread<T> object by move semantic
	*
	* \param other Thread to move into this
	*/

	Thread::Thread(Thread&& other) noexcept :
	m_impl(other.m_impl)
	{
		other.m_impl = nullptr;
	}

	/*!
	* \brief Waits that the thread ends and then destroys this
	*/

	Thread::~Thread()
	{
		if (m_impl)
		{
			m_impl->Join();
			delete m_impl;
			m_impl = nullptr;
		}
	}

	/*!
	* \brief Detaches the thread
	*/

	void Thread::Detach()
	{
		if (m_impl)
		{
			m_impl->Detach();
			delete m_impl;
			m_impl = nullptr;
		}
	}

	/*!
	* \brief Gets the id of the thread
	* \return The identifiant of the thread (PID)
	*/

	Thread::Id Thread::GetId() const
	{
		return Thread::Id(m_impl);
	}

	/*!
	* \brief Checks whether the thread can be joined
	* \return true if well formed and not detached
	*/

	bool Thread::IsJoinable() const
	{
		return m_impl != nullptr;
	}

	/*!
	* \brief Waits that the thread ends
	*
	* \remark Produce a NazaraError if no functor was assigned and NAZARA_CORE_SAFE is defined
	*/

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

	/*!
	* \brief Moves the other thread into this
	* \return A reference to this
	*
	* \param thread Thread to move in this
	*
	* \remark Produce a NazaraError if no functor was assigned and NAZARA_CORE_SAFE is defined
	* \remark And call std::terminate if no functor was assigned and NAZARA_CORE_SAFE is defined
	*/

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

	/*!
	* \brief Gets the number of simulatenous threads that can run on the same cpu
	* \return The number of simulatenous threads
	*/

	unsigned int Thread::HardwareConcurrency()
	{
		return HardwareInfo::GetProcessorCount();
	}

	/*!
	* \brief Makes sleep this thread
	*
	* \param milliseconds The number of milliseconds to sleep
	*/

	void Thread::Sleep(UInt32 milliseconds)
	{
		ThreadImpl::Sleep(milliseconds);
	}

	/*!
	* \brief Creates the implementation of this thread
	*
	* \param functor The task the thread will represent
	*/

	void Thread::CreateImpl(Functor* functor)
	{
		m_impl = new ThreadImpl(functor);
	}

	/*********************************Thread::Id********************************/

	/*!
	* \brief Constructs a Thread<T> object with a thread implementation
	*
	* \param thread Thread implementation assigned to the thread
	*/

	Thread::Id::Id(ThreadImpl* thread) :
	m_id(thread)
	{
	}

	/*!
	* \brief Compares two Thread::Id
	* \return true if the two thread ids are the same
	*
	* \param lhs First id
	* \param rhs Second id
	*/

	bool operator==(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id == rhs.m_id;
	}

	/*!
	* \brief Compares two Thread::Id
	* \return false if the two thread ids are the same
	*
	* \param lhs First id
	* \param rhs Second id
	*/

	bool operator!=(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id != rhs.m_id;
	}

	/*!
	* \brief Compares two Thread::Id
	* \return true if the first thread id is inferior to the second one
	*
	* \param lhs First id
	* \param rhs Second id
	*/

	bool operator<(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id < rhs.m_id;
	}

	/*!
	* \brief Compares two Thread::Id
	* \return true if the first thread id is inferior or equal to the second one
	*
	* \param lhs First id
	* \param rhs Second id
	*/

	bool operator<=(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id <= rhs.m_id;
	}

	/*!
	* \brief Compares two Thread::Id
	* \return true if the first thread id is superior to the second one
	*
	* \param lhs First id
	* \param rhs Second id
	*/

	bool operator>(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id > rhs.m_id;
	}

	/*!
	* \brief Compares two Thread::Id
	* \return true if the first thread id is superior or equal to the second one
	*
	* \param lhs First id
	* \param rhs Second id
	*/

	bool operator>=(const Thread::Id& lhs, const Thread::Id& rhs)
	{
		return lhs.m_id >= rhs.m_id;
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param id The thread id to output
	*/

	std::ostream& operator<<(std::ostream& o, const Nz::Thread::Id& id)
	{
		o << id.m_id;
		return o;
	}
}

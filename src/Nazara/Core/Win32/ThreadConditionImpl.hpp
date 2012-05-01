// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_THREADCONDITIONIMPL_HPP
#define NAZARA_THREADCONDITIONIMPL_HPP

// http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

#include <Nazara/Prerequesites.hpp>
#include <windows.h>

class NzMutexImpl;

class NzThreadConditionImpl
{
	public:
		NzThreadConditionImpl();
		#ifdef NAZARA_PLATFORM_WINDOWSVISTA
		~NzThreadConditionImpl() = default;
		#else
		~NzThreadConditionImpl();
		#endif

		void Signal();
		void SignalAll();

		void Wait(NzMutexImpl* mutex);
		bool Wait(NzMutexImpl* mutex, nzUInt32 timeout);

	private:
		#ifdef NAZARA_PLATFORM_WINDOWSVISTA
		CONDITION_VARIABLE m_cv;
		#else
		enum
		{
			SIGNAL = 0,
			BROADCAST = 1,
			MAX_EVENTS = 2
		};

		CRITICAL_SECTION m_countLock;
		HANDLE m_events[MAX_EVENTS];
		unsigned int m_count;
		#endif

};

#endif // NAZARA_THREADCONDITIONIMPL_HPP

// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

#pragma once

#ifndef NAZARA_CONDITIONVARIABLEIMPL_HPP
#define NAZARA_CONDITIONVARIABLEIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <windows.h>

class NzMutexImpl;

class NzConditionVariableImpl
{
	public:
		NzConditionVariableImpl();
		#if NAZARA_CORE_WINDOWS_VISTA
		~NzConditionVariableImpl() = default;
		#else
		~NzConditionVariableImpl();
		#endif

		void Signal();
		void SignalAll();

		void Wait(NzMutexImpl* mutex);
		bool Wait(NzMutexImpl* mutex, nzUInt32 timeout);

	private:
		#if NAZARA_CORE_WINDOWS_VISTA
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

#endif // NAZARA_CONDITIONVARIABLEIMPL_HPP

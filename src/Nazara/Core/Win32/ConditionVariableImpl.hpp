// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

#pragma once

#ifndef NAZARA_CONDITIONVARIABLEIMPL_HPP
#define NAZARA_CONDITIONVARIABLEIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <atomic>
#include <windows.h>

namespace Nz
{
	class MutexImpl;

	class ConditionVariableImpl
	{
		public:
			ConditionVariableImpl();
			#if NAZARA_CORE_WINDOWS_NT6
			~ConditionVariableImpl() = default;
			#else
			~ConditionVariableImpl();
			#endif

			void Signal();
			void SignalAll();

			void Wait(MutexImpl* mutex);
			bool Wait(MutexImpl* mutex, UInt32 timeout);

		private:
			#if NAZARA_CORE_WINDOWS_NT6
			CONDITION_VARIABLE m_cv;
			#else
			enum
			{
				SIGNAL,
				BROADCAST,
				MAX_EVENTS
			};

			std::atomic_uint m_count;
			HANDLE m_events[MAX_EVENTS];
			#endif
	};
}

#endif // NAZARA_CONDITIONVARIABLEIMPL_HPP

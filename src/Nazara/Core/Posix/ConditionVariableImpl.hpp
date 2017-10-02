// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

#pragma once

#ifndef NAZARA_CONDITIONVARIABLEIMPL_HPP
#define NAZARA_CONDITIONVARIABLEIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <pthread.h>

namespace Nz
{
	class MutexImpl;

	class ConditionVariableImpl
	{
		public:
			ConditionVariableImpl();
			~ConditionVariableImpl();

			void Signal();
			void SignalAll();

			void Wait(MutexImpl* mutex);
			bool Wait(MutexImpl* mutex, UInt32 timeout);

		private:
			pthread_cond_t m_cv;
	};
}

#endif // NAZARA_CONDITIONVARIABLEIMPL_HPP

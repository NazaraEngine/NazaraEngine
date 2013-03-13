// Copyright (C) 2013 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

#pragma once

#ifndef NAZARA_CONDITIONVARIABLEIMPL_HPP
#define NAZARA_CONDITIONVARIABLEIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

class NzMutexImpl;

class NzConditionVariableImpl
{
	public:
		NzConditionVariableImpl();
		~NzConditionVariableImpl();

		void Signal();
		void SignalAll();

		void Wait(NzMutexImpl* mutex);
		bool Wait(NzMutexImpl* mutex, nzUInt32 timeout);

	private:
		pthread_cond_t m_cv;
};

#endif // NAZARA_CONDITIONVARIABLEIMPL_HPP

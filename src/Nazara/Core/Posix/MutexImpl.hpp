// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

/*
    04/01/2012 : alexandre.janniaux@gmail.com
    Add pthread mutex implementation.
*/

#pragma once

#ifndef NAZARA_MUTEXIMPL_HPP
#define NAZARA_MUTEXIMPL_HPP

#include <pthread.h>

class NzMutexImpl
{
	friend class NzConditionVariableImpl;

	public:
		NzMutexImpl();
		~NzMutexImpl();

		void Lock();
		bool TryLock();
		void Unlock();

	private:
		pthread_mutex_t m_pmutex;
};

#endif // NAZARA_MUTEXIMPL_HPP

// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MUTEX_HPP
#define NAZARA_MUTEX_HPP

#include <Nazara/Prerequesites.hpp>

class NzMutexImpl;

class NAZARA_CORE_API NzMutex
{
	friend class NzConditionVariable;

	public:
		NzMutex();
        NzMutex(const NzMutex&) = delete;
        NzMutex(NzMutex&&) = delete; ///TODO
		~NzMutex();

		void Lock();
		bool TryLock();
		void Unlock();

        NzMutex& operator=(const NzMutex&) = delete;
        NzMutex& operator=(NzMutex&&) = delete; ///TODO

	private:
		NzMutexImpl* m_impl;
};

#endif // NAZARA_MUTEX_HPP

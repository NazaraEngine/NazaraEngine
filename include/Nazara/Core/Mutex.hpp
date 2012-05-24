// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MUTEX_HPP
#define NAZARA_MUTEX_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>

class NzMutexImpl;
class NzThreadCondition;

class NAZARA_API NzMutex : NzNonCopyable
{
	friend class NzThreadCondition;

	public:
		NzMutex();
		~NzMutex();

		void Lock();
		bool TryLock();
		void Unlock();

	private:
		NzMutexImpl* m_impl;
};

#endif // NAZARA_MUTEX_HPP

// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MUTEXIMPL_HPP
#define NAZARA_MUTEXIMPL_HPP

#include <windows.h>

class NzThreadConditionImpl;

class NzMutexImpl
{
	friend class NzThreadConditionImpl;

	public:
		NzMutexImpl();
		~NzMutexImpl();

		void Lock();
		bool TryLock();
		void Unlock();

	private:
		CRITICAL_SECTION m_criticalSection;
};

#endif // NAZARA_MUTEXIMPL_HPP

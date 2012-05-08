// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_THREADCONDITION_HPP
#define NAZARA_THREADCONDITION_HPP

#include <Nazara/Prerequesites.hpp>

class NzMutex;
class NzThreadConditionImpl;

class NAZARA_API NzThreadCondition
{
	public:
		NzThreadCondition();
		~NzThreadCondition();

		void Signal();
		void SignalAll();

		void Wait(NzMutex* mutex);
		bool Wait(NzMutex* mutex, nzUInt32 timeout);

	private:
		NzThreadConditionImpl* m_impl;
};

#endif // NAZARA_THREADCONDITION_HPP

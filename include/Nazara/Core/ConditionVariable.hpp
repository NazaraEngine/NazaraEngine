// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONDITIONVARIABLE_HPP
#define NAZARA_CONDITIONVARIABLE_HPP

#include <Nazara/Prerequesites.hpp>

class NzConditionVariableImpl;
class NzMutex;

class NAZARA_API NzConditionVariable
{
	public:
		NzConditionVariable();
		~NzConditionVariable();

		void Signal();
		void SignalAll();

		void Wait(NzMutex* mutex);
		bool Wait(NzMutex* mutex, nzUInt32 timeout);

	private:
		NzConditionVariableImpl* m_impl;
};

#endif // NAZARA_CONDITIONVARIABLE_HPP

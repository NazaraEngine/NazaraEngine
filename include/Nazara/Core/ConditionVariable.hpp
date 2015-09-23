// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONDITIONVARIABLE_HPP
#define NAZARA_CONDITIONVARIABLE_HPP

#include <Nazara/Prerequesites.hpp>

class NzConditionVariableImpl;
class NzMutex;

class NAZARA_CORE_API NzConditionVariable
{
	public:
		NzConditionVariable();
        NzConditionVariable(const NzConditionVariable&) = delete;
        NzConditionVariable(NzConditionVariable&&) = delete; ///TODO
		~NzConditionVariable();

		void Signal();
		void SignalAll();

		void Wait(NzMutex* mutex);
		bool Wait(NzMutex* mutex, nzUInt32 timeout);

        NzConditionVariable& operator=(const NzConditionVariable&) = delete;
        NzConditionVariable& operator=(NzConditionVariable&&) = delete; ///TODO

	private:
		NzConditionVariableImpl* m_impl;
};

#endif // NAZARA_CONDITIONVARIABLE_HPP

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONDITIONVARIABLE_HPP
#define NAZARA_CONDITIONVARIABLE_HPP

#include <Nazara/Prerequesites.hpp>

namespace Nz
{
	class ConditionVariableImpl;
	class Mutex;

	class NAZARA_CORE_API ConditionVariable
	{
		public:
			ConditionVariable();
			ConditionVariable(const ConditionVariable&) = delete;
			inline ConditionVariable(ConditionVariable&& condition) noexcept;
			~ConditionVariable();

			void Signal();
			void SignalAll();

			void Wait(Mutex* mutex);
			bool Wait(Mutex* mutex, UInt32 timeout);

			ConditionVariable& operator=(const ConditionVariable&) = delete;
			ConditionVariable& operator=(ConditionVariable&& condition) noexcept;

		private:
			ConditionVariableImpl* m_impl;
	};
}

#include <Nazara/Core/ConditionVariable.inl>

#endif // NAZARA_CONDITIONVARIABLE_HPP

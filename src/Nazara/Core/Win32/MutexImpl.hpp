// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MUTEXIMPL_HPP
#define NAZARA_MUTEXIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <windows.h>

namespace Nz
{
	class MutexImpl
	{
		friend class ConditionVariableImpl;

		public:
			MutexImpl();
			~MutexImpl();

			void Lock();
			bool TryLock();
			void Unlock();

		private:
			CRITICAL_SECTION m_criticalSection;
	};
}

#endif // NAZARA_MUTEXIMPL_HPP

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOCKGUARD_HPP
#define NAZARA_LOCKGUARD_HPP

#include <Nazara/Prerequesites.hpp>

namespace Nz
{
	class Mutex;

	class LockGuard
	{
		public:
			inline LockGuard(Mutex& mutex, bool lock = true);
			inline ~LockGuard();

			inline void Lock();
			inline bool TryLock();
			inline void Unlock();

		private:
			Mutex& m_mutex;
			bool m_locked;
	};
}

#include <Nazara/Core/LockGuard.inl>

#endif // NAZARA_LOCKGUARD_HPP

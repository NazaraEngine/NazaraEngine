// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOCKGUARD_HPP
#define NAZARA_LOCKGUARD_HPP

#include <Nazara/Prerequesites.hpp>

namespace Nz
{
	class Mutex;

	class NAZARA_CORE_API LockGuard
	{
		public:
			LockGuard(Mutex& mutex);
			~LockGuard();

		private:
			Mutex& m_mutex;
	};
}

#endif // NAZARA_LOCKGUARD_HPP

// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOCKGUARD_HPP
#define NAZARA_LOCKGUARD_HPP

#include <Nazara/Prerequesites.hpp>

class NzMutex;

class NAZARA_API NzLockGuard
{
	public:
		NzLockGuard(NzMutex& mutex);
		~NzLockGuard();

	private:
		NzMutex& m_mutex;
};

#endif // NAZARA_LOCKGUARD_HPP

// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOCK_HPP
#define NAZARA_LOCK_HPP

#include <Nazara/Prerequesites.hpp>

class NzMutex;

class NAZARA_API NzLock
{
	public:
		NzLock(NzMutex& mutex);
		~NzLock();

	private:
		NzMutex& m_mutex;
};

#endif // NAZARA_LOCK_HPP

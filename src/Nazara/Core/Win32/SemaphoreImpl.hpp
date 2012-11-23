// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SEMAPHOREIMPL_HPP
#define NAZARA_SEMAPHOREIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <windows.h>

class NzSemaphoreImpl
{
	public:
		NzSemaphoreImpl(unsigned int count);
		~NzSemaphoreImpl();

		unsigned int GetCount() const;
		void Post();
		void Wait();
		bool Wait(nzUInt32 timeout);

	private:
		HANDLE m_semaphore;
};

#endif // NAZARA_SEMAPHOREIMPL_HPP

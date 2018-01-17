// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SEMAPHOREIMPL_HPP
#define NAZARA_SEMAPHOREIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <semaphore.h>

namespace Nz
{
	class SemaphoreImpl
	{
		public:
			SemaphoreImpl(unsigned int count);
			~SemaphoreImpl();

			unsigned int GetCount() const;
			void Post();
			void Wait();
			bool Wait(UInt32 timeout);

		private:
			sem_t m_semaphore;
	};
}

#endif // NAZARA_SEMAPHOREIMPL_HPP

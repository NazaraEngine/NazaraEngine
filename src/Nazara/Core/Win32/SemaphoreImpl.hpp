// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SEMAPHOREIMPL_HPP
#define NAZARA_SEMAPHOREIMPL_HPP

#include <Nazara/Prerequisites.hpp>
#include <windows.h>

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
			HANDLE m_semaphore;
	};
}

#endif // NAZARA_SEMAPHOREIMPL_HPP

// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SEMAPHORE_HPP
#define NAZARA_SEMAPHORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/MovablePtr.hpp>

namespace Nz
{
	class SemaphoreImpl;

	class NAZARA_CORE_API Semaphore
	{
		public:
			Semaphore(unsigned int count);
			Semaphore(const Semaphore&) = delete;
			Semaphore(Semaphore&&) noexcept = default;
			~Semaphore();

			unsigned int GetCount() const;

			void Post();

			void Wait();
			bool Wait(UInt32 timeout);

			Semaphore& operator=(const Semaphore&) = delete;
			Semaphore& operator=(Semaphore&&) noexcept = default;

		private:
			MovablePtr<SemaphoreImpl> m_impl;
	};
}

#endif // NAZARA_SEMAPHORE_HPP

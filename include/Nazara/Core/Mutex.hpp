// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MUTEX_HPP
#define NAZARA_MUTEX_HPP

#include <Nazara/Prerequesites.hpp>

namespace Nz
{
	class MutexImpl;

	class NAZARA_CORE_API Mutex
	{
		friend class ConditionVariable;

		public:
			Mutex();
			Mutex(const Mutex&) = delete;
			inline Mutex(Mutex&& mutex) noexcept;
			~Mutex();

			void Lock();
			bool TryLock();
			void Unlock();

			Mutex& operator=(const Mutex&) = delete;
			Mutex& operator=(Mutex&& mutex) noexcept;

		private:
			MutexImpl* m_impl;
	};
}

#include <Nazara/Core/Mutex.inl>

#endif // NAZARA_MUTEX_HPP

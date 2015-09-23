// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SEMAPHORE_HPP
#define NAZARA_SEMAPHORE_HPP

#include <Nazara/Prerequesites.hpp>

class NzSemaphoreImpl;

class NAZARA_CORE_API NzSemaphore
{
	public:
		NzSemaphore(unsigned int count);
		NzSemaphore(const NzSemaphore&) = delete;
		NzSemaphore(NzSemaphore&&) = delete; ///TODO
		~NzSemaphore();

		unsigned int GetCount() const;

		void Post();

		void Wait();
		bool Wait(nzUInt32 timeout);

		NzSemaphore& operator=(const NzSemaphore&) = delete;
		NzSemaphore& operator=(NzSemaphore&&) = delete; ///TODO

	private:
		NzSemaphoreImpl* m_impl;
};

#endif // NAZARA_SEMAPHORE_HPP

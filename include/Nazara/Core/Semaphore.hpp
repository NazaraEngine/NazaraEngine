// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SEMAPHORE_HPP
#define NAZARA_SEMAPHORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/NonCopyable.hpp>

class NzSemaphoreImpl;

class NAZARA_API NzSemaphore : NzNonCopyable
{
	public:
		NzSemaphore(unsigned int count);
		~NzSemaphore();

		unsigned int GetCount() const;
		void Post();
		void Wait();
		bool Wait(nzUInt32 timeout);

	private:
		NzSemaphoreImpl* m_impl;
};

#endif // NAZARA_SEMAPHORE_HPP

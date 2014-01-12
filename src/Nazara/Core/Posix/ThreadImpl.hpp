// Copyright (C) 2014 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_THREADIMPL_HPP
#define NAZARA_THREADIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <pthread.h>

struct NzFunctor;

class NzThreadImpl
{
	public:
		NzThreadImpl(NzFunctor* threadFunc);

		void Detach();
		void Join();

		static void Sleep(nzUInt32 time);

	private:
		static void* ThreadProc(void* userdata);

		pthread_t m_handle;
};

#endif // NAZARA_THREADIMPL_HPP

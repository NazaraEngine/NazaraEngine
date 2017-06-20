// Copyright (C) 2015 Alexandre Janniaux
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_THREADIMPL_HPP
#define NAZARA_THREADIMPL_HPP

#include <Nazara/Prerequesites.hpp>

#ifdef __GNUC__
#define _GNU_SOURCE
#endif

#include <pthread.h>

namespace Nz
{
	struct Functor;

	class ThreadImpl
	{
		public:
			ThreadImpl(Functor* threadFunc);

			void Detach();
			void Join();
			void SetName(const Nz::String& name);

			static void SetCurrentName(const Nz::String& name);
			static void Sleep(UInt32 time);

		private:
			static void* ThreadProc(void* userdata);

			pthread_t m_handle;
	};
}

#endif // NAZARA_THREADIMPL_HPP

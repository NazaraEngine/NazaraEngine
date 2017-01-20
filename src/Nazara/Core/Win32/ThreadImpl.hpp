// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Inspiré du code de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_THREADIMPL_HPP
#define NAZARA_THREADIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <windows.h>

namespace Nz
{
	struct Functor;

	class ThreadImpl
	{
		public:
			ThreadImpl(Functor* threadFunc);

			void Detach();
			void Join();

			static void Sleep(UInt32 time);

		private:
			static unsigned int __stdcall ThreadProc(void* userdata);

			HANDLE m_handle;
	};
}

#endif // NAZARA_THREADIMPL_HPP

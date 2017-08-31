// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Inspiré du code de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_THREADIMPL_HPP
#define NAZARA_THREADIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
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
			void SetName(const Nz::String& name);

			static void SetCurrentName(const Nz::String& name);
			static void Sleep(UInt32 time);

		private:
			static void SetThreadName(DWORD threadId, const char* threadName);
			static unsigned int __stdcall ThreadProc(void* userdata);

			DWORD m_threadId;
			HANDLE m_handle;
	};
}

#endif // NAZARA_THREADIMPL_HPP

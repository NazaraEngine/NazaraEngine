// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Inspiré du code de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_THREADIMPL_HPP
#define NAZARA_THREADIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <windows.h>

struct NzFunctor;

class NzThreadImpl
{
	public:
		NzThreadImpl(NzFunctor* threadFunc);

		void Detach();
		void Join();

		static void Sleep(nzUInt32 time);

	private:
		static unsigned int __stdcall ThreadProc(void* userdata);

		HANDLE m_handle;
};

#endif // NAZARA_THREADIMPL_HPP

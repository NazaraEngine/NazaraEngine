// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

// Inspiré du code de la SFML par Laurent Gomila

#pragma once

#ifndef NAZARA_THREADIMPL_HPP
#define NAZARA_THREADIMPL_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Thread.hpp>
#include <windows.h>

class NzThread;

class NzThreadImpl
{
	friend class NzThread::Id;

	public:
		NzThreadImpl(NzThread* threadFunc);
		~NzThreadImpl();

		NzThread::Id GetId() const;
		bool IsCurrent() const;
		void Join();
		void Terminate();

	private:
		static unsigned int __stdcall ThreadProc(void* userdata);

		HANDLE m_thread;
		unsigned int m_threadId;
};

#endif // NAZARA_THREADIMPL_HPP

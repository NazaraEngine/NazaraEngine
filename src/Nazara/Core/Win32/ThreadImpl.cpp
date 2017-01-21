// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/ThreadImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Functor.hpp>
#include <process.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	ThreadImpl::ThreadImpl(Functor* functor)
	{
		m_handle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &ThreadImpl::ThreadProc, functor, 0, nullptr));
		if (!m_handle)
			NazaraInternalError("Failed to create thread: " + Error::GetLastSystemError());
	}

	void ThreadImpl::Detach()
	{
		// http://stackoverflow.com/questions/418742/is-it-reasonable-to-call-closehandle-on-a-thread-before-it-terminates
		CloseHandle(m_handle);
	}

	void ThreadImpl::Join()
	{
		WaitForSingleObject(m_handle, INFINITE);
		CloseHandle(m_handle);
	}

	unsigned int __stdcall ThreadImpl::ThreadProc(void* userdata)
	{
		Functor* func = static_cast<Functor*>(userdata);
		func->Run();
		delete func;

		/*
		En C++, il vaut mieux retourner depuis la fonction que de quitter le thread explicitement
		Source : http://msdn.microsoft.com/en-us/library/windows/desktop/ms682659(v=vs.85).aspx
		*/

		return 0;
	}

	void ThreadImpl::Sleep(UInt32 time)
	{
		::Sleep(time);
	}
}

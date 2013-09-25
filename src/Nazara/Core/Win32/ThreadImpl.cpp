// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/ThreadImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Functor.hpp>
#include <process.h>
#include <Nazara/Core/Debug.hpp>

NzThreadImpl::NzThreadImpl(NzFunctor* functor)
{
	m_handle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &NzThreadImpl::ThreadProc, functor, 0, nullptr));
	if (!m_handle)
		NazaraInternalError("Failed to create thread: " + NzError::GetLastSystemError());
}

void NzThreadImpl::Detach()
{
	// http://stackoverflow.com/questions/418742/is-it-reasonable-to-call-closehandle-on-a-thread-before-it-terminates
	CloseHandle(m_handle);
}

void NzThreadImpl::Join()
{
	WaitForSingleObject(m_handle, INFINITE);
	CloseHandle(m_handle);
}

unsigned int __stdcall NzThreadImpl::ThreadProc(void* userdata)
{
	NzFunctor* func = static_cast<NzFunctor*>(userdata);
	func->Run();
	delete func;

	/*
	En C++, il vaut mieux retourner depuis la fonction que de quitter le thread explicitement
	Source : http://msdn.microsoft.com/en-us/library/windows/desktop/ms682659(v=vs.85).aspx
	*/

	return 0;
}

void NzThreadImpl::Sleep(nzUInt32 time)
{
	::Sleep(time);
}

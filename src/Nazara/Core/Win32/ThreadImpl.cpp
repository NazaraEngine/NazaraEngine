// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Inspiré du code de la SFML par Laurent Gomila

#include <Nazara/Core/Win32/ThreadImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Functor.hpp>
#include <process.h>
#include <Nazara/Core/Debug.hpp>

NzThread::Id::Id(const NzThreadImpl* thread)
{
	if (thread->m_thread)
		m_handle = reinterpret_cast<void*>(thread->m_threadId); // Un entier transformé en pointeur : Hacky
	else
		m_handle = nullptr;
}

NzThread::Id::~Id()
{
}

bool NzThread::Id::operator==(const Id& rhs) const
{
	return m_handle == rhs.m_handle;
}

bool NzThread::Id::operator!=(const Id& rhs) const
{
	return m_handle != rhs.m_handle;
}

NzThreadImpl::NzThreadImpl(NzThread* thread)
{
	m_thread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &NzThreadImpl::ThreadProc, thread, 0, &m_threadId));
	if (!m_thread)
		NazaraError("Failed to create thread: " + NzGetLastSystemError());
}

NzThreadImpl::~NzThreadImpl()
{
}

NzThread::Id NzThreadImpl::GetId() const
{
	return NzThread::Id(reinterpret_cast<void*>(m_threadId)); // Hacky
}

bool NzThreadImpl::IsCurrent() const
{
	return m_threadId == GetCurrentThreadId();
}

void NzThreadImpl::Join()
{
	if (m_thread)
		WaitForSingleObject(m_thread, INFINITE);
}

void NzThreadImpl::Terminate()
{
	if (m_thread)
		TerminateThread(m_thread, 0);
}

unsigned int __stdcall NzThreadImpl::ThreadProc(void* userdata)
{
	NzThread* owner = reinterpret_cast<NzThread*>(userdata);
	NzFunctor* func = owner->m_func;
	HANDLE myHandle = owner->m_impl->m_thread;
	func->Run();
	delete func;

	// http://stackoverflow.com/questions/418742/is-it-reasonable-to-call-closehandle-on-a-thread-before-it-terminates
	CloseHandle(myHandle);

	/*
	En C++, il vaut mieux retourner depuis la fonction que de quitter le thread explicitement
	Source : http://msdn.microsoft.com/en-us/library/windows/desktop/ms682659(v=vs.85).aspx
	*/
	return 0;
}

NzThread::Id NzThread::GetCurrentId()
{
	return NzThread::Id(reinterpret_cast<void*>(GetCurrentThreadId())); // Hacky
}

void NzThread::Sleep(nzUInt32 time)
{
	::Sleep(time);
}

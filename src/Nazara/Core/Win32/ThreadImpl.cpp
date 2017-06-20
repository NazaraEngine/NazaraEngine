// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/ThreadImpl.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Functor.hpp>
#include <process.h>
#include <windows.h>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace
	{
#pragma pack(push,8)
		struct THREADNAME_INFO
		{
			DWORD dwType;
			LPCSTR szName;
			DWORD dwThreadID;
			DWORD dwFlags;
		};
#pragma pack(pop)
	}

	ThreadImpl::ThreadImpl(Functor* functor)
	{
		m_handle = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &ThreadImpl::ThreadProc, functor, 0, &m_threadId));
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

	void ThreadImpl::SetName(const Nz::String& name)
	{
		SetThreadName(m_threadId, name.GetConstBuffer());
	}

	void ThreadImpl::SetCurrentName(const Nz::String& name)
	{
		SetThreadName(::GetCurrentThreadId(), name.GetConstBuffer());
	}

	void ThreadImpl::Sleep(UInt32 time)
	{
		::Sleep(time);
	}

	void ThreadImpl::SetThreadName(DWORD threadId, const char* threadName)
	{
		// https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
		constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = threadId;
		info.dwFlags = 0;

#pragma warning(push)
#pragma warning(disable: 6320 6322)
		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
#pragma warning(pop)
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
}

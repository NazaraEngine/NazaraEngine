// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Win32/ThreadImpl.hpp>
#include <Nazara/Core/Format.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz::PlatformImpl
{
#ifndef NAZARA_COMPILER_MINGW_THREADS_POSIX
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		// Windows 10, version 1607 added GetThreadDescription and SetThreadDescription in order to name a thread
		using GetThreadDescriptionFunc = HRESULT(WINAPI*)(HANDLE hThread, PWSTR* ppszThreadDescription);
		using SetThreadDescriptionFunc = HRESULT(WINAPI*)(HANDLE hThread, PCWSTR lpThreadDescription);

#ifdef NAZARA_COMPILER_MSVC
#pragma pack(push,8)
		struct THREADNAME_INFO
		{
			DWORD dwType;
			LPCSTR szName;
			DWORD dwThreadID;
			DWORD dwFlags;
		};
#pragma pack(pop)
#endif // NAZARA_COMPILER_MSVC
	}
#endif // !NAZARA_COMPILER_MINGW

	ThreadHandle GetCurrentThreadHandle()
	{
#ifndef NAZARA_COMPILER_MINGW_THREADS_POSIX
		return ::GetCurrentThread();
#else
		return ::pthread_self();
#endif
	}

	std::string GetCurrentThreadName()
	{
		return GetThreadName(GetCurrentThreadHandle());
	}

	std::string GetThreadName(ThreadHandle threadHandle)
	{
#ifndef NAZARA_COMPILER_MINGW_THREADS_POSIX
		NAZARA_USE_ANONYMOUS_NAMESPACE

		// Use GetThreadDescription if available
		PWSTR namePtr;
		static GetThreadDescriptionFunc GetThreadDescription = reinterpret_cast<GetThreadDescriptionFunc>(::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "GetThreadDescription"));
		if (!GetThreadDescription)
			return "<GetThreadDescription not supported>";

		HRESULT hr = GetThreadDescription(threadHandle, &namePtr);
		if (FAILED(hr))
			return Format("<GetThreadDescription failed: {}>", HRESULT_CODE(hr));

		CallOnExit freeName([&] { LocalFree(namePtr); });

		return FromWideString(namePtr);
#else
		std::array<char, 16> name;
		::pthread_getname_np(threadHandle, &name[0], name.size());

		return std::string(&name[0]);
#endif
	}

#ifndef NAZARA_COMPILER_MINGW_THREADS_POSIX
	void RaiseThreadNameException(DWORD threadId, const char* threadName)
	{
#ifdef NAZARA_COMPILER_MSVC
		NAZARA_USE_ANONYMOUS_NAMESPACE

		if (!::IsDebuggerPresent())
			return;

		// https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
		constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = threadId;
		info.dwFlags = 0;

NAZARA_WARNING_PUSH()
NAZARA_WARNING_MSVC_DISABLE(6320 6322)
		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
NAZARA_WARNING_POP()

#else
		NazaraUnused(threadId);
		NazaraUnused(threadName);
		NazaraWarning("ThreadName exception is only supported with MSVC");
#endif
	}
#endif

	void SetCurrentThreadName(const char* threadName)
	{
		SetThreadName(GetCurrentThreadHandle(), threadName);
	}

	void SetThreadName(ThreadHandle threadHandle, const char* threadName)
	{
#ifndef NAZARA_COMPILER_MINGW_THREADS_POSIX
		NAZARA_USE_ANONYMOUS_NAMESPACE

		// Try to use SetThreadDescription if available
		static SetThreadDescriptionFunc SetThreadDescription = reinterpret_cast<SetThreadDescriptionFunc>(::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "SetThreadDescription"));
		if (SetThreadDescription)
			SetThreadDescription(threadHandle, ToWideString(threadName).data());
#if NAZARAUTILS_WINDOWS_NT6
		else
			RaiseThreadNameException(::GetThreadId(threadHandle), threadName);
#endif

#else
		::pthread_setname_np(threadHandle, threadName);
#endif
	}
}

#ifndef NAZARA_COMPILER_MINGW_THREADS_POSIX
#include <Nazara/Core/AntiWindows.hpp>
#endif
